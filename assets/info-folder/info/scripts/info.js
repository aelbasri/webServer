// Add interactivity or animations here
document.addEventListener("DOMContentLoaded", function () {
    const sections = document.querySelectorAll("section");

    sections.forEach((section, index) => {
        section.style.opacity = 0;
        section.style.transform = "translateY(20px)";
        setTimeout(() => {
            section.style.transition = "opacity 1s ease, transform 1s ease";
            section.style.opacity = 1;
            section.style.transform = "translateY(0)";
        }, index * 300); // Staggered animation
    });
});
