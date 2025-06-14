document.addEventListener("DOMContentLoaded", () => {
  const headers = document.querySelectorAll("h1");

  headers.forEach(h1 => {
    const text = h1.textContent;
    h1.textContent = ""; 

    let i = 0;
    function type() {
      if (i < text.length) {
        h1.textContent += text.charAt(i);
        i++;
        setTimeout(type, 100); 
      }
    }

    type();
  });
});

