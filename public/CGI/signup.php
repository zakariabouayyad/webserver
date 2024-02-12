<?php
session_start();
if (isset($_SESSION['user'])) {
    header("Location: admin/index.php");
    exit();
}
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // $queryString = $_SERVER['QUERY_STRING'];
    // parse_str($queryString, $_POST);
    // Validate the form data (you can add more validation as needed)
    $name = $_POST['name'];
    $username = $_POST['username'];
    $password = $_POST['password'];


    // Hash the password (use a secure hashing algorithm like bcrypt)
    

    // Save user information to a text file (for demonstration purposes)
    $filePath = 'users.txt';
    
    if (!file_exists($filePath)) {
        // Create the file with a default user for protection (replace with your actual default user)
        $defaultUser = "admin|admin|". password_hash("admin", PASSWORD_BCRYPT) . "\n";
        file_put_contents($filePath, $defaultUser);
    }
    $userData = file('users.txt', FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);

    foreach ($userData as $line) {
        list(, $savedUsername, ) = explode('|', $line);
        
        if ($username === $savedUsername) {
            $error = 'Username already exists.';
            break;
        }
    }
    if (!isset($error)) {
    $hashedPassword = password_hash($password, PASSWORD_BCRYPT);
    $userData = "$name|$username|$hashedPassword" . PHP_EOL;
    file_put_contents('users.txt', $userData, FILE_APPEND);

    // Redirect to login page or any other destination
    header('Location: signin.php');
    exit();
    }
}
?>
<!DOCTYPE html>
<html lang="en" >
<head>
  <meta charset="UTF-8">
  <title>Sign Up</title>
  <link href="https://fonts.googleapis.com/css?family=Open+Sans:400,700" rel="stylesheet"><link rel="stylesheet" href="./style.css">
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.5.1/css/all.min.css" />

</head>
<body>
<!-- partial:index.partial.html -->
<body class="align">
<input type="checkbox" id="active">
    <label for="active" class="menu-btn"><i class="fas fa-bars"></i></label>
    <div class="wrapper">
      <ul>
<li><a href="/CGI/">Home</a></li>
<li><a href="signin.php">Login</a></li>
<li><a href="py.py">Python</a></li>
<li><a href="php.php">PHP</a></li>
</ul>
</div>
  <div class="grid">
    <form method="POST" class="form login">
      <?php if(isset($error)) { echo "<p class='form__field'>$error</p>"; }
        else {  echo "<p class='form__field'></p>" ;} ?>
      <div class="form__field">
        <label for="login__username"><svg class="icon"><use xmlns:xlink="http://www.w3.org/1999/xlink" xlink:href="#user"></use></svg><span class="hidden">Username</span></label>
        <input id="login__username" type="text" name="name" class="form__input" placeholder="Name" required>
      </div>
      <div class="form__field">
        <label for="login__username"><svg class="icon"><use xmlns:xlink="http://www.w3.org/1999/xlink" xlink:href="#user"></use></svg><span class="hidden">Username</span></label>
        <input id="login__username" type="text" name="username" class="form__input" placeholder="Username" required>
      </div>
      <div class="form__field">
        <label for="login__password"><svg class="icon"><use xmlns:xlink="http://www.w3.org/1999/xlink" xlink:href="#lock"></use></svg><span class="hidden">Password</span></label>
          <input id="login__password" type="password" name="password" class="form__input" placeholder="Password" required>
      </div>
      <div class="form__field">
        <input type="submit" value="Sign Up">
      </div>
    </form>
    <p class="text--center">Already a member? <a href="signin.php">Sign in now</a></p>
  </div>
  <svg xmlns="http://www.w3.org/2000/svg" class="icons"><symbol id="arrow-right" viewBox="0 0 1792 1792"><path d="M1600 960q0 54-37 91l-651 651q-39 37-91 37-51 0-90-37l-75-75q-38-38-38-91t38-91l293-293H245q-52 0-84.5-37.5T128 1024V896q0-53 32.5-90.5T245 768h704L656 474q-38-36-38-90t38-90l75-75q38-38 90-38 53 0 91 38l651 651q37 35 37 90z"/></symbol><symbol id="lock" viewBox="0 0 1792 1792"><path d="M640 768h512V576q0-106-75-181t-181-75-181 75-75 181v192zm832 96v576q0 40-28 68t-68 28H416q-40 0-68-28t-28-68V864q0-40 28-68t68-28h32V576q0-184 132-316t316-132 316 132 132 316v192h32q40 0 68 28t28 68z"/></symbol><symbol id="user" viewBox="0 0 1792 1792"><path d="M1600 1405q0 120-73 189.5t-194 69.5H459q-121 0-194-69.5T192 1405q0-53 3.5-103.5t14-109T236 1084t43-97.5 62-81 85.5-53.5T538 832q9 0 42 21.5t74.5 48 108 48T896 971t133.5-21.5 108-48 74.5-48 42-21.5q61 0 111.5 20t85.5 53.5 62 81 43 97.5 26.5 108.5 14 109 3.5 103.5zm-320-893q0 159-112.5 271.5T896 896 624.5 783.5 512 512t112.5-271.5T896 128t271.5 112.5T1280 512z"/></symbol></svg>
</body>
<!-- partial -->
  
</body>
</html>