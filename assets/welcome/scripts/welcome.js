// DOM Elements and Initialization
document.addEventListener('DOMContentLoaded', function() {
    // Initialize the server animation
    initServerAnimation();
    
    // Initialize the step cards animation
    animateStepCards();
    
    // Add hover effects to navigation links
    addNavigationEffects();
    
    // Add feature pill animations
    animateFeaturePills();
});

// Server Animation
function initServerAnimation() {
    // Add random blinking to server units
    const serverUnits = document.querySelectorAll('.server-unit');
    
    serverUnits.forEach(unit => {
        if (!unit.classList.contains('blink')) {
            // Randomly add blinking to non-blinking units
            setInterval(() => {
                const shouldBlink = Math.random() > 0.7;
                if (shouldBlink) {
                    unit.classList.add('temp-blink');
                    setTimeout(() => {
                        unit.classList.remove('temp-blink');
                    }, 500);
                }
            }, 3000);
        }
    });
    
    // Add a pulsing effect to the server rack
    const serverRack = document.querySelector('.server-rack');
    if (serverRack) {
        setInterval(() => {
            serverRack.style.transform = 'scale(1.02)';
            setTimeout(() => {
                serverRack.style.transform = 'scale(1)';
            }, 200);
        }, 5000);
    }
}

// Step Cards Animation
function animateStepCards() {
    const stepCards = document.querySelectorAll('.step-card');
    
    stepCards.forEach((card, index) => {
        // Add a staggered animation delay for initial load
        setTimeout(() => {
            card.style.opacity = '0';
            card.style.transform = 'translateY(20px)';
            
            // Trigger animation
            setTimeout(() => {
                card.style.transition = 'opacity 0.5s ease, transform 0.5s ease';
                card.style.opacity = '1';
                card.style.transform = 'translateY(0)';
            }, 100);
        }, index * 200);
    });
}

// Navigation Effects
function addNavigationEffects() {
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
}

// Feature Pills Animation
function animateFeaturePills() {
    const featurePills = document.querySelectorAll('.feature-pill');
    
    featurePills.forEach((pill, index) => {
        // Add a staggered animation delay
        setTimeout(() => {
            pill.style.opacity = '0';
            pill.style.transform = 'scale(0.8)';
            
            // Trigger animation
            setTimeout(() => {
                pill.style.transition = 'all 0.5s ease';
                pill.style.opacity = '1';
                pill.style.transform = 'scale(1)';
            }, 50);
        }, index * 100);
    });
    
    // Add hover interaction
    featurePills.forEach(pill => {
        pill.addEventListener('mouseenter', function() {
            const randomColor = getRandomColor();
            this.style.background = `linear-gradient(135deg, ${randomColor.start}, ${randomColor.end})`;
        });
        
        pill.addEventListener('mouseleave', function() {
            this.style.background = 'linear-gradient(135deg, var(--primary-color), var(--secondary-color))';
        });
    });
}

// Utility: Get random color for gradients
function getRandomColor() {
    const colors = [
        { start: '#4285f4', end: '#34a853' }, // Default
        { start: '#ea4335', end: '#fbbc05' }, // Red to Yellow
        { start: '#4285f4', end: '#ea4335' }, // Blue to Red
        { start: '#34a853', end: '#fbbc05' }, // Green to Yellow
        { start: '#673ab7', end: '#3f51b5' }, // Purple to Blue
        { start: '#009688', end: '#4caf50' }  // Teal to Green
    ];
    
    return colors[Math.floor(Math.random() * colors.length)];
}

// Add a typing animation to code snippets
setTimeout(() => {
    const codeBlocks = document.querySelectorAll('.code-snippet code');
    
    codeBlocks.forEach(code => {
        const text = code.textContent;
        code.textContent = '';
        
        let i = 0;
        const typeWriter = () => {
            if (i < text.length) {
                code.textContent += text.charAt(i);
                i++;
                setTimeout(typeWriter, 5);
            }
        };
        
        // Start typing animation when in viewport
        const observer = new IntersectionObserver(entries => {
            entries.forEach(entry => {
                if (entry.isIntersecting) {
                    typeWriter();
                    observer.unobserve(entry.target);
                }
            });
        });
        
        observer.observe(code);
    });
}, 1000);
