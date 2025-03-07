// JavaScript client that sends a file to a Python CGI script
document.getElementById('uploadForm').addEventListener('submit', function(e) {
    e.preventDefault();
    
    const fileInput = document.getElementById('fileInput');
    const file = fileInput.files[0];
    
    if (file) {
      sendToPythonCGI(file);
    } else {
      alert('Please select a file to upload');
    }
  });
  
  /**
   * Send the file to a Python CGI script that will handle the chunked upload
   * @param {File} file - The file to upload
   */
  function sendToPythonCGI(file) {
    const statusElement = document.getElementById('status');
    const progressBar = document.getElementById('progressBar');
    
    // URL of your Python CGI script
    const cgiUrl = '/cgi-bin/upload_handler.py';
    
    // Create FormData to send the file
    const formData = new FormData();
    formData.append('file', file);
    
    // Add command to use chunked transfer encoding
    formData.append('use_chunked_encoding', 'true');
    
    // Set up request
    const xhr = new XMLHttpRequest();
    xhr.open('POST', cgiUrl, true);
    
    // Set up progress tracking
    xhr.upload.onprogress = function(e) {
      if (e.lengthComputable) {
        const percent = Math.round((e.loaded / e.total) * 100);
        console.log(`Upload progress: ${percent}%`);
        
        if (progressBar) {
          progressBar.value = percent;
        }
      }
    };
    
    // Handle completion
    xhr.onload = function() {
      if (xhr.status >= 200 && xhr.status < 300) {
        const response = JSON.parse(xhr.responseText);
        console.log('Upload response:', response);
        
        if (statusElement) {
          if (response.success) {
            statusElement.textContent = 'File successfully uploaded and processed by Python CGI!';
          } else {
            statusElement.textContent = 'Python CGI reported an error: ' + response.error;
          }
        }
      } else {
        console.error('Upload failed with status:', xhr.status);
        if (statusElement) {
          statusElement.textContent = 'Upload failed: ' + xhr.statusText;
        }
      }
    };
    
    xhr.onerror = function() {
      console.error('Network error during upload');
      if (statusElement) {
        statusElement.textContent = 'Network error during upload';
      }
    };
    
    // Send the request to the Python CGI script
    xhr.send(formData);
  }