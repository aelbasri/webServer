// DOM Elements
document.addEventListener('DOMContentLoaded', function() {
    // Set up the POST form submission
    const postForm = document.getElementById('post-form');
    if (postForm) {
        postForm.addEventListener('submit', function(e) {
            e.preventDefault();
            testPost();
        });
    }
    
    // Initialize any animations or effects
    animateFeatureCards();
});

// Test GET request
function testGet() {
    const resultElement = document.getElementById('get-result');
    resultElement.textContent = "Sending GET request...";
    
    // Simulate a delay for the request
    setTimeout(() => {
        resultElement.textContent = `GET Request Successful
Status: 200 OK
Time: ${new Date().toLocaleTimeString()}

Response:
{
  "message": "This is a GET response from WebServ42",
  "status": "success"
}`;
    }, 500);
}

// Test POST request
function testPost() {
    const resultElement = document.getElementById('post-result');
    const form = document.getElementById('post-form');
    const formData = new FormData(form);
    const data = formData.get('data') || 'No data provided';
    
    resultElement.textContent = "Sending POST request...";
    
    // Simulate a delay for the request
    setTimeout(() => {
        resultElement.textContent = `POST Request Successful
Status: 201 Created
Time: ${new Date().toLocaleTimeString()}

Request Data:
"${data}"

Response:
{
  "message": "Data received successfully",
  "received": "${data}",
  "status": "success"
}`;
    }, 700);
}

// Test DELETE request
function testDelete() {
    const resultElement = document.getElementById('delete-result');
    const resourcePath = document.getElementById('delete-path').value || '/resource/default';
    
    resultElement.textContent = "Sending DELETE request...";
    
    // Simulate a delay for the request
    setTimeout(() => {
        resultElement.textContent = `DELETE Request Successful
Status: 204 No Content
Time: ${new Date().toLocaleTimeString()}

Resource deleted: ${resourcePath}`;
    }, 600);
}

// Animation for feature cards
function animateFeatureCards() {
    const featureCards = document.querySelectorAll('.feature-card');
    
    featureCards.forEach((card, index) => {
        // Add a staggered animation delay
        setTimeout(() => {
            card.style.opacity = '0';
            card.style.transform = 'translateY(20px)';
            
            // Trigger animation
            setTimeout(() => {
                card.style.transition = 'opacity 0.5s ease, transform 0.5s ease';
                card.style.opacity = '1';
                card.style.transform = 'translateY(0)';
            }, 100);
        }, index * 150);
    });
}

// Add hover effects to navigation links
const navLinks = document.querySelectorAll('nav ul li a');
navLinks.forEach(link => {
    link.addEventListener('mouseenter', function() {
        this.style.transition = 'all 0.3s ease';
        this.style.transform = 'translateY(-2px)';
    });
    
    link.addEventListener('mouseleave', function() {
        this.style.transition = 'all 0.3s ease';
        this.style.transform = 'translateY(0)';
    });
});
