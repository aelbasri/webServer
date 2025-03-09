document.addEventListener('DOMContentLoaded', function() {
  // Add 3D parallax effect to cards
  const cards = document.querySelectorAll('.card');
  const members = document.querySelectorAll('.member');
  const elements = [...cards, ...members];
  
  // Initialize parallax effect
  initParallax();
  
  // Add scroll animations
  window.addEventListener('scroll', revealElements);
  revealElements(); // Check if any elements are visible on initial load
  
  // Add mouseover effects to team members
  members.forEach(member => {
    member.addEventListener('mouseover', pulseAvatar);
    member.addEventListener('mouseout', resetAvatar);
  });
  
  // Add click effect to all social buttons (GitHub and LinkedIn)
  const socialButtons = document.querySelectorAll('.social-btn');
  socialButtons.forEach(button => {
    button.addEventListener('click', function(e) {
      // Don't prevent default anymore, just add the ripple effect
      clickEffect.call(this, e);
    });
  });
  
  // Make cube responsive to mouse movements
  const cube = document.querySelector('.rotating-cube');
  document.addEventListener('mousemove', function(e) {
    const x = e.clientX / window.innerWidth;
    const y = e.clientY / window.innerHeight;
    
    // Calculate rotation based on mouse position
    const rotateX = (y - 0.5) * 60; // -30 to 30 degrees
    const rotateY = (x - 0.5) * 60; // -30 to 30 degrees
    
    // Apply rotation to cube while maintaining spin animation
    cube.style.transform = `rotateX(${rotateX}deg) rotateY(${rotateY}deg)`;
    
    // Keep the faces spinning
    const faces = document.querySelectorAll('.cube-face');
    faces.forEach(face => {
      face.style.animationPlayState = 'running';
    });
  });
  
  // Easter egg - spin the cube faster when clicking on title
  const title = document.getElementById('main-title');
  
  title.addEventListener('click', function() {
    cube.style.animation = 'rotate 5s linear infinite';
    
    setTimeout(() => {
      cube.style.animation = 'rotate 20s linear infinite';
    }, 3000);
  });
  
  // Functions
  function initParallax() {
    document.addEventListener('mousemove', function(e) {
      const x = e.clientX / window.innerWidth;
      const y = e.clientY / window.innerHeight;
      
      elements.forEach(element => {
        const depth = 30; // Increased depth for more 3D effect
        const moveX = (x - 0.5) * depth;
        const moveY = (y - 0.5) * depth;
        
        element.style.transform = `translateX(${moveX}px) translateY(${moveY}px) translateZ(15px)`; // Increased Z value
      });
    });
  }
  
  function revealElements() {
    const windowHeight = window.innerHeight;
    const revealPoint = 150;
    
    elements.forEach(element => {
      const elementTop = element.getBoundingClientRect().top;
      
      if (elementTop < windowHeight - revealPoint) {
        element.style.opacity = 1;
        element.style.transform = 'translateY(0) translateZ(20px)'; // Increased Z value
      } else {
        element.style.opacity = 0;
        element.style.transform = 'translateY(50px) translateZ(20px)'; // Increased Z value
      }
    });
  }
  
  function pulseAvatar() {
    const avatar = this.querySelector('.github-avatar');
    if (avatar) {
      avatar.style.transform = 'scale(1.1)';
      avatar.style.boxShadow = '0 0 30px rgba(0, 195, 255, 0.8)';
    }
  }
  
  function resetAvatar() {
    const avatar = this.querySelector('.github-avatar');
    if (avatar) {
      avatar.style.transform = 'scale(1)';
      avatar.style.boxShadow = '0 0 20px rgba(0, 195, 255, 0.5)';
    }
  }
  
  function clickEffect(e) {
    // Create ripple effect
    const button = this;
    const ripple = document.createElement('span');
    
    const rect = button.getBoundingClientRect();
    const size = Math.max(rect.width, rect.height);
    
    const x = e.clientX - rect.left - size / 2;
    const y = e.clientY - rect.top - size / 2;
    
    ripple.style.width = ripple.style.height = `${size}px`;
    ripple.style.left = `${x}px`;
    ripple.style.top = `${y}px`;
    ripple.className = 'ripple';
    
    button.appendChild(ripple);
    
    setTimeout(() => {
      ripple.remove();
    }, 600);
  }
  
  // Add type writer effect to the main title
  let i = 0;
  const titleElement = document.getElementById('main-title');
  const titleText = titleElement.innerText;
  
  // Clear the title initially
  titleElement.innerText = '';
  
  // Type effect function
  function typeWriter() {
    if (i < titleText.length) {
      titleElement.innerText += titleText.charAt(i);
      i++;
      setTimeout(typeWriter, 150);
    }
  }
  
  // Start the type writer effect
  setTimeout(typeWriter, 500);
  
  // Add 3D tilt effect to the cards
  cards.forEach(card => {
    card.addEventListener('mousemove', tiltEffect);
    card.addEventListener('mouseleave', resetTilt);
  });
  
  function tiltEffect(e) {
    const cardRect = this.getBoundingClientRect();
    const x = e.clientX - cardRect.left;
    const y = e.clientY - cardRect.top;
    
    const centerX = cardRect.width / 2;
    const centerY = cardRect.height / 2;
    
    const tiltX = (y - centerY) / 8; // Increased tilt effect
    const tiltY = (centerX - x) / 8; // Increased tilt effect
    
    this.style.transform = `rotateX(${tiltX}deg) rotateY(${tiltY}deg) translateZ(20px)`; // Increased Z value
  }
  
  function resetTilt() {
    this.style.transform = 'rotateX(0) rotateY(0) translateZ(20px)'; // Increased Z value
  }

  // FIXED HTTP method testing functionality
  // Add event listeners for HTTP method testing buttons
  const testGetBtn = document.getElementById('test-get');
  const testPostBtn = document.getElementById('test-post');
  const testDeleteBtn = document.getElementById('test-delete');

  if (testGetBtn) testGetBtn.addEventListener('click', testGetMethod);
  if (testPostBtn) testPostBtn.addEventListener('click', testPostMethod);
  if (testDeleteBtn) testDeleteBtn.addEventListener('click', testDeleteMethod);

  // Apply ripple effect to method buttons
  const methodBtns = document.querySelectorAll('.method-btn');
  methodBtns.forEach(button => {
    button.addEventListener('click', clickEffect);
  });

  // Functions for HTTP method testing
  function testGetMethod() {
    const url = document.getElementById('get-url').value;
    const resultBox = document.getElementById('get-result');

    console.log(`url is: ${url}`)
    
    // Display loading message
    resultBox.innerHTML = '<p>Sending GET request...</p>';
    resultBox.classList.add('active');
    
    // Send the actual GET request using fetch
    fetch(url)
        .then(response => {
            // Check if the response is OK (status code 200-299)
            if (!response.ok) {
                throw new Error(`HTTP error! Status: ${response.status}`);
            }
            return response.text(); // Parse the response as text
        })
        .then(data => {
            // Display the successful response
            resultBox.innerHTML = `
                <p><strong>Status:</strong> 200 OK</p>
                <p><strong>Headers:</strong> ${url}</p>
            `;
        })
        .catch(error => {
            // Handle errors (e.g., network issues, invalid URL, etc.)
            resultBox.innerHTML = `
                <p><strong>Error:</strong> ${error.message}</p>
                <p><strong>URL:</strong> ${url}</p>
            `;
        });
    }

  function testPostMethod() {
    const url = document.getElementById('post-url').value;
    const data = document.getElementById('post-data').value;
    const resultBox = document.getElementById('post-result');
    
    resultBox.innerHTML = '<p>Sending POST request...</p>';
    resultBox.classList.add('active');
    
    // Send the actual POST request using fetch
    fetch(url, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json', // Adjust the content type as needed
        },
        body: data, // Send data directly, no need to stringify twice
    })
    .then(response => {
        if (!response.ok) {
            throw new Error(`HTTP error! Status: ${response.status}`);
        }
        return response.text(); // Parse the response as text
    })
    .then(responseData => {
        // Display the successful response
        resultBox.innerHTML = `
            <p><strong>Status:</strong> 200 OK</p>
            <p><strong>URL:</strong> ${url}</p>
            <p><strong>Data sent:</strong></p>
            <pre>${data || '(empty)'}</pre>
            <p><strong>Response:</strong></p>
            <pre>${responseData}</pre>
        `;
    })
    .catch(error => {
        // Handle errors (e.g., network issues, invalid URL, etc.)
        resultBox.innerHTML = `
            <p><strong>Error:</strong> ${error.message}</p>
            <p><strong>URL:</strong> ${url}</p>
            <p><strong>Data sent:</strong></p>
            <pre>${data || '(empty)'}</pre>
        `;
    });
  }
    function testDeleteMethod() {
      const url = document.getElementById('delete-url').value;
      const resultBox = document.getElementById('delete-result');
      
      resultBox.innerHTML = '<p>Sending DELETE request...</p>';
      resultBox.classList.add('active');
      
      // Send the actual DELETE request using fetch
      fetch(url, {
          method: 'DELETE',
      })
      .then(response => {
          if (!response.ok) {
              throw new Error(`HTTP error! Status: ${response.status}`);
          }
          return response.text(); // Parse the response as text
      })
      .then(responseData => {
          // Display the successful response
          resultBox.innerHTML = `
              <p><strong>Status:</strong> 200 OK</p>
              <p><strong>URL:</strong> ${url}</p>
              <p><strong>Response:</strong></p>
              <pre>${responseData}</pre>
          `;
      })
      .catch(error => {
          // Handle errors (e.g., network issues, invalid URL, etc.)
          resultBox.innerHTML = `
              <p><strong>Error:</strong> ${error.message}</p>
              <p><strong>URL:</strong> ${url}</p>
          `;
      });
  }
});