<?php
  if ($_SERVER['REQUEST_METHOD'] === "GET"){
    if (isset($_GET["name"]) && isset($_GET["last_name"])){
        $name = $_GET["name"];
        $last_name = $_GET["last_name"];
        $title = "PHP GET";
    }
}
else{
    if (isset($_POST["name"]) && isset($_POST["last_name"])){
        $name = $_POST["name"];
        $last_name = $_POST["last_name"];
        $title = "PHP POST";
    }
}
?>
<!DOCTYPE html>
<html lang="en" >
<head>
  <meta charset="UTF-8">
  <title>PHP</title>
  <link href="https://fonts.googleapis.com/css?family=Open+Sans:400,700" rel="stylesheet"><link rel="stylesheet" href="./style.css">
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.5.1/css/all.min.css" />
  <link rel="icon" href="https://www.php.net/favicon.ico" type="image/x-icon">

</head>
<body>
<!-- partial:index.partial.html -->
<body class="align" id="yes">
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
<img src="https://upload.wikimedia.org/wikipedia/commons/thumb/2/27/PHP-logo.svg/1280px-PHP-logo.svg.png">
<div class="parent">
  <div class="grid" id="block">
  <h1 class="text--center"><?php if (isset($title)) echo $title; else echo "PHP"; ?></h1>
  <p class="text--center" id="pnono">First Name: <span class="spanclass"><?php if (isset($name)) echo $name; else echo "............"; ?></span></p>
  <p class="text--center" id="pnono">Last  Name: <span class="spanclass"><?php if (isset($last_name)) echo $last_name; else echo "............"; ?></span></p>
  </div>
  </div>
  <div class="parent">
  <div class="grid" id="block">
    <form method="POST" class="form login" action="./php.php">
      <div class="form__field">
        <label for="login__username"><svg class="icon"><use xmlns:xlink="http://www.w3.org/1999/xlink" xlink:href="#user"></use></svg><span class="hidden">Username</span></label>
        <input id="login__username" type="text" name="name" class="form__input" placeholder="First Name" required>
      </div>
      <div class="form__field">
        <label for="login__username"><svg class="icon"><use xmlns:xlink="http://www.w3.org/1999/xlink" xlink:href="#user"></use></svg><span class="hidden">Username</span></label>
        <input id="login__username" type="text" name="last_name" class="form__input" placeholder="Last Name" required>
      </div>
      <div class="form__field">
        <input type="submit" value="POST">
      </div>
    </form>
  </div>
  <div class="grid" id="block">
    <form method="GET" class="form login">
      <div class="form__field">
        <label for="login__username"><svg class="icon"><use xmlns:xlink="http://www.w3.org/1999/xlink" xlink:href="#user"></use></svg><span class="hidden">Username</span></label>
        <input id="login__username" type="text" name="name" class="form__input" placeholder="First Name" required>
      </div>
      <div class="form__field">
        <label for="login__username"><svg class="icon"><use xmlns:xlink="http://www.w3.org/1999/xlink" xlink:href="#user"></use></svg><span class="hidden">Username</span></label>
        <input id="login__username" type="text" name="last_name" class="form__input" placeholder="Last Name" required>
      </div>
      <div class="form__field">
        <input type="submit" value="GET">
      </div>
    </form>
  </div>
  </div>
  <svg xmlns="http://www.w3.org/2000/svg" class="icons"><symbol id="arrow-right" viewBox="0 0 1792 1792"><path d="M1600 960q0 54-37 91l-651 651q-39 37-91 37-51 0-90-37l-75-75q-38-38-38-91t38-91l293-293H245q-52 0-84.5-37.5T128 1024V896q0-53 32.5-90.5T245 768h704L656 474q-38-36-38-90t38-90l75-75q38-38 90-38 53 0 91 38l651 651q37 35 37 90z"/></symbol><symbol id="lock" viewBox="0 0 1792 1792"><path d="M640 768h512V576q0-106-75-181t-181-75-181 75-75 181v192zm832 96v576q0 40-28 68t-68 28H416q-40 0-68-28t-28-68V864q0-40 28-68t68-28h32V576q0-184 132-316t316-132 316 132 132 316v192h32q40 0 68 28t28 68z"/></symbol><symbol id="user" viewBox="0 0 1792 1792"><path d="M1600 1405q0 120-73 189.5t-194 69.5H459q-121 0-194-69.5T192 1405q0-53 3.5-103.5t14-109T236 1084t43-97.5 62-81 85.5-53.5T538 832q9 0 42 21.5t74.5 48 108 48T896 971t133.5-21.5 108-48 74.5-48 42-21.5q61 0 111.5 20t85.5 53.5 62 81 43 97.5 26.5 108.5 14 109 3.5 103.5zm-320-893q0 159-112.5 271.5T896 896 624.5 783.5 512 512t112.5-271.5T896 128t271.5 112.5T1280 512z"/></symbol></svg>
</body>
<!-- partial -->
  
</body>
</html>