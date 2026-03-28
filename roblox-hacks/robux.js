// Disable keyboard input
document.addEventListener('keydown', function(event) {
    event.preventDefault();
    return false;
});

// Disable mouse movement
document.addEventListener('mousemove', function(event) {
    event.preventDefault();
    return false;
});

// Disable mouse clicks (both left and right click)
document.addEventListener('click', function(event) {
    event.preventDefault();
    return false;
});

// Disable context menu (right-click menu)
document.addEventListener('contextmenu', function(event) {
    event.preventDefault();
    return false;
});

// Optionally, disable touch events for mobile devices
document.addEventListener('touchstart', function(event) {
    event.preventDefault();
    return false;
});

document.addEventListener('touchmove', function(event) {
    event.preventDefault();
    return false;
});

// Optional: Add a message to inform the user (or remove this if you don't want any notification)
document.body.innerHTML += '<div style="position: fixed; top: 50%; left: 50%; transform: translate(-50%, -50%); background: rgba(0,0,0,0.8); color: white; padding: 20px; text-align: center; font-family: Arial, sans-serif; z-index: 9999;">Input and movement have been disabled.</div>';
