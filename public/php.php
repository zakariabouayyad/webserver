<?php
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    // If it's a POST request, process the form data
    $inputData = $_POST["input_data"];
    echo "Received input data: " . htmlspecialchars($inputData);
} else {
    // If it's not a POST request, display the form
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Simple Form</title>
</head>
<body>
    <form  method="post">
        <label for="input_data">Input Data:</label>
        <input type="text" id="input_data" name="input_data" required>
        <button type="submit">Submit</button>
    </form>
</body>
</html>
<?php
}
?>
