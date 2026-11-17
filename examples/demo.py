"""Open a fictional JAyuda guide with a page served by a loopback container."""

from contextlib import contextmanager
from pathlib import Path
import shutil
import socket
import subprocess
import tempfile
import time


PYTHON = 'python@sha256:5e2bdb1b0d542d5039cffb8181c65930093949194a65c365dfb1b0fe13a4fa5b'
HERE = Path(__file__).resolve().parent
START = '''<!doctype html>
<html lang="es"><head><meta charset="utf-8"><title>Guía de demostración | JAyuda</title></head>
<body style="font:16px Arial;padding:20px;color:#15314a;background:#f3f7fa">
<h1>Guía de demostración</h1>
<p>Aprende a consultar los pasos para preparar un envío ficticio. Referencia DEMO-104.</p>
<iframe src="{endpoint}/ficha.html" title="Ficha ficticia servida por HTTP" width="480" height="180"></iframe>
</body></html>'''
PAGE = '''<!doctype html>
<html lang="es"><head><meta charset="utf-8"><title>Ficha de ejemplo</title></head>
<body style="font:16px Arial;padding:12px;color:#15314a;background:#e1f2ea">
<h2>Preparar un envío de muestra</h2>
<ol><li>Crea la ficha DEMO-104.</li><li>Selecciona el destino ficticio Isla Verde.</li></ol>
</body></html>'''
INDEX = r'''<?xml version="1.0" encoding="utf-8"?>
<lista><pagina><titulo>Guia de demostracion</titulo><path>ayuda\inicio.html</path></pagina></lista>
'''


def docker(*args, check=True):
    return subprocess.run(['docker', *args], capture_output=True, text=True,
                          encoding='utf-8', check=check, timeout=30)


@contextmanager
def fixture(binary_path):
    if not (binary_path / 'JAyuda.exe').is_file():
        raise FileNotFoundError('Build JAyuda Release first / Compila primero JAyuda Release')
    with tempfile.TemporaryDirectory(prefix='jayuda-demo-') as folder:
        root = Path(folder)
        site = root / 'site'
        site.mkdir()
        (site / 'ficha.html').write_text(PAGE, encoding='utf-8')
        http = docker('run', '--rm', '-d', '--name', 'prueba-jayuda-http',
                      '-p', '127.0.0.1::8080', '--mount',
                      f'type=bind,source={site},target=/site,readonly', PYTHON,
                      'python', '-u', '-m', 'http.server', '8080', '--directory', '/site').stdout.strip()
        try:
            address = docker('port', http, '8080/tcp').stdout.strip()
            host, port = address.split(':')
            if host != '127.0.0.1' or not port.isdigit():
                raise RuntimeError('Expected an IPv4 loopback port')
            for _ in range(24):
                try:
                    with socket.create_connection((host, int(port)), timeout=0.5):
                        break
                except OSError:
                    time.sleep(0.25)
            else:
                raise RuntimeError('The sample HTTP server did not start')
            app = root / 'app'
            help_folder = app / 'ayuda' / 'ayuda'
            help_folder.mkdir(parents=True)
            shutil.copy2(binary_path / 'JAyuda.exe', app)
            for dll in binary_path.glob('*.dll'):
                shutil.copy2(dll, app)
            welcome = START.format(endpoint=f'http://{address}')
            for directory in (help_folder, help_folder.parent):
                (directory / 'inicio.html').write_text(welcome, encoding='utf-8')
            (help_folder.parent / 'index.xml').write_text(INDEX, encoding='utf-8')
            yield app, http, address
        finally:
            docker('stop', '--time', '2', http, check=False)


def main():
    with fixture(HERE.parent / 'bin/Release') as (app, server, address):
        print(f'Demo HTTP on {address}. Open Ayuda in JAyuda, then close its window.', flush=True)
        subprocess.run([app / 'JAyuda.exe'], cwd=app, check=True)
        response = docker('logs', server)
        print(response.stdout + response.stderr)


if __name__ == '__main__':
    main()
