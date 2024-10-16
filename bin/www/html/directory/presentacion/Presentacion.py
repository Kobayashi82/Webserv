#!/usr/bin/python3

print("Content-Type: text/html\r\n\r")

print("""
<!DOCTYPE html>
<html>

<head>
    <meta charset="UTF-8">
    <title>Webserv 1.0</title>
    <style>
        body {
            margin: 0;
            padding: 0;
            height: 100vh;
            display: flex;
            align-items: center;
            justify-content: center;
            font-family: Tahoma, Verdana, Arial, sans-serif;
            background: linear-gradient(135deg, #8b7346, #703b47);
            color: #fff;
        }

        .container {
            text-align: center;
            background-color: rgba(26, 193, 223, 0.1);
            padding: 2em;
            border-radius: 15px;
            box-shadow: 0 8px 16px rgba(0, 0, 0, 0.2);
            backdrop-filter: blur(10px);
        }

        h1 {
            font-size: 2.5em;
            margin-bottom: 0.5em;
        }

        p {
            font-size: 1.2em;
            line-height: 1.6em;
        }

		.small-text {
            font-size: 0.9em;
            color: #e0e0e0;
        }

    </style>
</head>

<body>
    <div class="container">
        <h1>Bienvenido a Webserv 1.0</h1>
        <p>Un servidor web cutre, pero hecho con cariño 😊</p>
		<p class="small-text">Esta página es un CGI de python</p>
    </div>
</body>

</html>
""")