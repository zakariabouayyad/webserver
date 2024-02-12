import cgi
import os

# Set content type to HTML
print("Content-type: text/html; charset=UTF-8\r\n")
# Get form data
form = cgi.FieldStorage()
# Get request method
names = 0
last_names = 0
request_method = os.environ.get('REQUEST_METHOD', 'GET')
if form.length :
    if (request_method == "GET"):
        names = form.getlist('name')
        last_names = form.getlist('last_name')
        title = "PYTHON GET"

    else:
        names = form.getlist('namepost')
        last_names = form.getlist('last_namepost')
        title = "PYTHON POST"


# Start HTML response
print("\r\n\r\n")


print("<!DOCTYPE html>")
print("<html lang='en' >")
print("<head>")
print("  <meta charset='UTF-8'>")
print("  <title>Python</title>")
print("  <link href='https://fonts.googleapis.com/css?family=Open+Sans:400,700' rel='stylesheet'><link rel='stylesheet' href='./style.css'>")
print("  <link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.5.1/css/all.min.css' />")
print("    <link rel='icon' href='https://www.python.org/static/favicon.ico' type='image/x-icon'>")
print("")
print("</head>")
print("<body>")
print("<!-- partial:index.partial.html -->")
print("<body class='align' id='yes'>")
print("<input type='checkbox' id='active'>")
print("    <label for='active' class='menu-btn'><i class='fas fa-bars'></i></label>")
print("    <div class='wrapper'>")
print("      <ul>")
print("<li><a href='/CGI/'>Home</a></li>")
print("<li><a href='signin.php'>Login</a></li>")
print("<li><a href='py.py'>Python</a></li>")
print("<li><a href='php.php'>PHP</a></li>")
print("</ul>")
print("</div>")
print("<img src='https://cdn3.iconfinder.com/data/icons/logos-and-brands-adobe/512/267_Python-512.png'>")
print("<div class='parent'>")
print("  <div class='grid' id='block'>")
if (not(names and last_names)):
    print("  <h1 class='text--center'>PYTHON</h1>")   
    print("  <p class='text--center' id='pnono'>First Name: <span class='spanclass'>............</span></p>")
    print("  <p class='text--center' id='pnono'>Last  Name: <span class='spanclass'>............</span></p>")
else:
    print("  <h1 class='text--center'>{}</h1>".format(title))
    print("  <p class='text--center' id='pnono'>First Name: <span class='spanclass'>{}</span></p>".format(", ".join(names)))
    print("  <p class='text--center' id='pnono'>Last  Name: <span class='spanclass'>{}</span></p>".format(", ".join(last_names)))
print("  </div>")
print("  </div>")
print("  <div class='parent'>")
print("  <div class='grid' id='block'>")
print("    <form method='POST' class='form login' action='./py.py'>")
print("      <div class='form__field'>")
print("        <label for='login__username'><svg class='icon'><use xmlns:xlink='http://www.w3.org/1999/xlink' xlink:href='#user'></use></svg><span class='hidden'>Username</span></label>")
print("        <input id='login__username' type='text' name='namepost' class='form__input' placeholder='First Name' required>")
print("      </div>")
print("      <div class='form__field'>")
print("        <label for='login__username'><svg class='icon'><use xmlns:xlink='http://www.w3.org/1999/xlink' xlink:href='#user'></use></svg><span class='hidden'>Username</span></label>")
print("        <input id='login__username' type='text' name='last_namepost' class='form__input' placeholder='Last Name' required>")
print("      </div>")
print("      <div class='form__field'>")
print("        <input type='submit' value='POST'>")
print("      </div>")
print("    </form>")
print("  </div>")
print("  <div class='grid' id='block'>")
print("    <form method='GET' class='form login'>")
print("      <div class='form__field'>")
print("        <label for='login__username'><svg class='icon'><use xmlns:xlink='http://www.w3.org/1999/xlink' xlink:href='#user'></use></svg><span class='hidden'>Username</span></label>")
print("        <input id='login__username' type='text' name='name' class='form__input' placeholder='First Name' required>")
print("      </div>")
print("      <div class='form__field'>")
print("        <label for='login__username'><svg class='icon'><use xmlns:xlink='http://www.w3.org/1999/xlink' xlink:href='#user'></use></svg><span class='hidden'>Username</span></label>")
print("        <input id='login__username' type='text' name='last_name' class='form__input' placeholder='Last Name' required>")
print("      </div>")
print("      <div class='form__field'>")
print("        <input type='submit' value='GET'>")
print("      </div>")
print("    </form>")
print("  </div>")
print("  </div>")
print("  <svg xmlns='http://www.w3.org/2000/svg' class='icons'><symbol id='arrow-right' viewBox='0 0 1792 1792'><path d='M1600 960q0 54-37 91l-651 651q-39 37-91 37-51 0-90-37l-75-75q-38-38-38-91t38-91l293-293H245q-52 0-84.5-37.5T128 1024V896q0-53 32.5-90.5T245 768h704L656 474q-38-36-38-90t38-90l75-75q38-38 90-38 53 0 91 38l651 651q37 35 37 90z'/></symbol><symbol id='lock' viewBox='0 0 1792 1792'><path d='M640 768h512V576q0-106-75-181t-181-75-181 75-75 181v192zm832 96v576q0 40-28 68t-68 28H416q-40 0-68-28t-28-68V864q0-40 28-68t68-28h32V576q0-184 132-316t316-132 316 132 132 316v192h32q40 0 68 28t28 68z'/></symbol><symbol id='user' viewBox='0 0 1792 1792'><path d='M1600 1405q0 120-73 189.5t-194 69.5H459q-121 0-194-69.5T192 1405q0-53 3.5-103.5t14-109T236 1084t43-97.5 62-81 85.5-53.5T538 832q9 0 42 21.5t74.5 48 108 48T896 971t133.5-21.5 108-48 74.5-48 42-21.5q61 0 111.5 20t85.5 53.5 62 81 43 97.5 26.5 108.5 14 109 3.5 103.5zm-320-893q0 159-112.5 271.5T896 896 624.5 783.5 512 512t112.5-271.5T896 128t271.5 112.5T1280 512z'/></symbol></svg>")
print("</body>")
print("<!-- partial -->")
print("  ")
print("</body>")
print("</html>")

# import  requests
# requests.get("http://localhost:8080/CGI/py.py")