# JAyuda
[English](#english) · [Español](#español)

## English


Legacy Windows help viewer with a topic index, keyword search and links between guide pages

> [!WARNING]
> **Status: Legacy.** This is a Windows MFC application with the system’s embedded MSHTML browser.

### Overview

JAyuda displays a collection of help pages in a desktop window. A topic tree organizes the guides, and keyword search shows which local pages contain the terms you entered. The original viewer can also display a web page linked by the guide.

### Features

- Open the welcome page and browse a topic tree defined in `index.xml`.
- Search indexed local HTML files and rank results by matching lines.
- Use Back, Forward, Home and Refresh to move through the guide. This version uses a ribbon with a Navigation tab for moving between pages. It retains the index and search tabs in the left pane.

### Requirements

The 2026 reconstruction converted the archived projects for Win32 and added compatibility files in compat/. It prepared the portable settings profiles and the fallback that uses executable metadata. The fictional guide and the local HTTP launcher in examples/ also come from 2026.

Use Visual Studio with Desktop development with C++, Win32 MFC and MSVC v145. The Windows 11 build used Visual Studio Community 2026 18.11.4, MSBuild 18.11.1.51303, MSVC 14.51.36231 and Windows SDK 10.0.26100.0. The build targets Win32 with x64 host tools. The archived browser uses MSHTML and the application packages its own Hhelp 1.0.0.1, IOFile 2.0.1.1, LAppc 1.1.0.1, Conector 3.0.0.1 and Componentes 2.1.0.1 from the `libreria2` source snapshot. The default portable edition runs without a serial or `appt.dll`. Each app version keeps an independent Windows profile. The original registration checks remain under `ENABLE_LICENSE_CHECK`. The library sources carry the backup date May 26, 2020 UTC. LAppc 1.1.0.1 supplies the portable application name and resolves help files beside the executable so the viewer can start without its original installation module.

The archived Win32 MySQL 5.0.16 client and headers come with the repository under vendor/mysql-sdk/. The viewer links the client through its shared library. The sample guide uses its local HTTP server. The exact archived controls are under vendor/third-party/controls/. Their original notices and hashes are recorded. The demo was verified with Python 3.12.4 and Docker Engine 29.9.2, using a Python 3.12.16 HTTP container.

### Getting started

Run from the repository root in Developer PowerShell for Visual Studio. The script copies the build to a temporary location, writes a fictional guide and starts a disposable HTTP server published only on loopback. It releases both when you close JAyuda.

```powershell
$env:MySqlSdkDir = (Resolve-Path .\vendor\mysql-sdk).Path
MSBuild JAyuda.sln /m:4 /nr:false /t:Rebuild /p:Configuration=Release /p:Platform=Win32 /p:PreferredToolArchitecture=x64
Copy-Item "$env:MySqlSdkDir\libmysql.dll" .\bin\Release\
python -X utf8 examples/demo.py
```

### Usage

Select **Ayuda** to open the demonstration guide. The topic index lists **Guía de demostración**. The embedded example shows how to prepare a fictional shipment. Open **Buscar**, enter `DEMO-104` and press **Buscar**. The list shows the guide with a score of `1`.

### Project structure

`app/` contains the window code and archived Visual Studio project. `vendor/` contains the recovered shared-library sources with their original and converted projects. `JAyuda.sln` builds the Win32 viewer and those libraries. `compat/` redirects the old theme header name to the current Windows SDK. `examples/demo.py` creates the guide and its local HTTP server.

### History

The executable resource identifies this archived version as 2.0.0.1. This version uses a ribbon with a Navigation tab for moving between pages. It retains the index and search tabs in the left pane. The application source folders carry the backup date January 7, 2020 UTC. The original edit dates are unknown.

### Archive variants

The project resource identifies this snapshot as 2.0.0.1. Consult the first commit for 1.1.0.1 with its classic toolbar. The two portable builds keep separate Windows preferences so their docking layouts do not conflict.

### Contributing

When reporting an issue, include the snapshot version, the build command and the fictional page or query that reproduced it.

### Support

Use Issues for reproducible build or viewing problems.

### License

The original code uses [MIT](LICENSE). The [third-party notices](THIRD-PARTY-NOTICES.md) identify their components.

---

## Español


Visor de ayuda legado para Windows con índice de temas, búsqueda por palabras y enlaces entre páginas

> [!WARNING]
> **Estado: legado.** Es una aplicación MFC para Windows con el navegador MSHTML integrado del sistema.

### Descripción general

JAyuda muestra una colección de páginas de ayuda en una ventana de escritorio. Un árbol de temas permite encontrar una guía. La búsqueda muestra qué páginas locales contienen las palabras ingresadas. El visor original también puede mostrar contenido web enlazado desde la guía.

### Funciones

- Abre la página de bienvenida y recorre un árbol de temas definido en `index.xml`.
- Busca en archivos HTML locales del índice y ordena los resultados por líneas coincidentes.
- Usa Atrás, Adelante, Inicio y Actualizar para recorrer la guía. Esta versión usa una cinta con la pestaña Navegación para moverse entre páginas. Conserva el índice y la búsqueda en el panel izquierdo.

### Requisitos

La reconstrucción de 2026 convirtió los proyectos archivados para Win32 y agregó los archivos de compatibilidad de compat/. Preparó los perfiles de preferencias portátiles y la alternativa que usa los metadatos del ejecutable. La guía ficticia y el lanzador HTTP local de examples/ son de 2026.

Usa Visual Studio con Desarrollo para el escritorio con C++, MFC para Win32 y MSVC v145. La compilación en Windows 11 usó Visual Studio Community 2026 18.11.4, MSBuild 18.11.1.51303, MSVC 14.51.36231 y Windows SDK 10.0.26100.0. El destino es Win32 con herramientas anfitrionas x64. El navegador histórico usa MSHTML. Se incluyen las bibliotecas propias Hhelp 1.0.0.1, IOFile 2.0.1.1, LAppc 1.1.0.1, Conector 3.0.0.1 y Componentes 2.1.0.1 del respaldo `libreria2`. La edición portátil arranca sin serial ni `appt.dll`. Cada versión guarda su propio perfil de Windows. Las comprobaciones originales quedan bajo `ENABLE_LICENSE_CHECK`. Los fuentes de las bibliotecas llevan la fecha de respaldo del 26 de mayo de 2020 en UTC. LAppc 1.1.0.1 proporciona el nombre portátil de la aplicación y busca la ayuda junto al ejecutable para que el visor arranque sin el módulo de instalación original.

El cliente Win32 MySQL 5.0.16 y sus cabeceras archivadas vienen en vendor/mysql-sdk/. El visor enlaza el cliente mediante su biblioteca. La guía de muestra usa el servidor HTTP local. Los controles archivados exactos están en vendor/third-party/controls/ con sus avisos y hashes. La demo se verificó con Python 3.12.4 y Docker Engine 29.9.2 y un contenedor HTTP con Python 3.12.16.

### Cómo empezar

Ejecuta desde la raíz en Developer PowerShell para Visual Studio. El script copia la compilación a una carpeta temporal, escribe una guía ficticia y arranca un servidor HTTP desechable en la interfaz local. Los retira al cerrar JAyuda.

```powershell
$env:MySqlSdkDir = (Resolve-Path .\vendor\mysql-sdk).Path
MSBuild JAyuda.sln /m:4 /nr:false /t:Rebuild /p:Configuration=Release /p:Platform=Win32 /p:PreferredToolArchitecture=x64
Copy-Item "$env:MySqlSdkDir\libmysql.dll" .\bin\Release\
python -X utf8 examples/demo.py
```

### Uso

Selecciona **Ayuda** para abrir la guía de demostración. El índice contiene **Guía de demostración**. La ficha incorporada muestra cómo preparar un envío ficticio. Abre **Buscar**, escribe `DEMO-104` y pulsa **Buscar**. La lista muestra la guía con puntaje `1`.

### Estructura del proyecto

`app/` contiene el código de la ventana y el proyecto original de Visual Studio. `vendor/` contiene los fuentes recuperados de las bibliotecas, con los proyectos originales y convertidos. `JAyuda.sln` compila el visor Win32 y sus bibliotecas. `compat/` conecta el nombre antiguo del encabezado de temas con el SDK actual de Windows. `examples/demo.py` genera la guía y su servidor HTTP local.

### Historia

El recurso del ejecutable identifica esta versión del respaldo como 2.0.0.1. Esta versión usa una cinta con la pestaña Navegación para moverse entre páginas. Conserva el índice y la búsqueda en el panel izquierdo. Las carpetas de la aplicación llevan la fecha de respaldo del 7 de enero de 2020 en UTC. Las fechas originales de edición se desconocen.

### Versiones del respaldo

El recurso del proyecto identifica esta copia como 2.0.0.1. Consulta el primer commit para ver la 1.1.0.1 con su barra clásica. Las dos compilaciones portátiles guardan sus preferencias de Windows por separado para evitar conflictos entre los diseños del panel.

### Contribuir

Al reportar un problema, incluye la versión del respaldo, el comando de compilación y la página ficticia o consulta que lo reprodujo.

### Soporte

Usa Issues para problemas reproducibles de compilación o visualización.

### Licencia

El código propio usa [MIT](LICENSE). Los [avisos de terceros](THIRD-PARTY-NOTICES.md) identifican sus componentes.
