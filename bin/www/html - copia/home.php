<?php
session_start();

// Comprobar si la cookie de sesión existe
if (isset($_COOKIE['user_session'])) {
    echo "<h1>Bienvenido a la página de inicio</h1>";
    echo "<p>Usuario autenticado: " . htmlspecialchars($_COOKIE['user_session']) . "</p>";
    echo "<a href='/logout.php'>Cerrar sesión</a>";
} else {
    // Si no existe la cookie, redirigir al login
    header('Location: /login.php');
    exit();
}
?>