## To build the docs:

Install the following

- python3
- pip3
- install mkdocs + extensions
- If you are under MSYS2, make sure to install libcrypt-devel & python3:

# MSYS2:

```bash
pacman -S libcrypt-devel
pacman -S python3
curl https://bootstrap.pypa.io/get-pip.py -o get-pip.py
python3 get-pip.py
echo 'export PATH=/home/eran/.local/bin;$PATH' >> ~/.bashrc
. ~/.bashrc
```

# Ubuntu / Debian:

```bash
# make sure we got curl installed
sudo apt-get install curl
# install pip3
curl https://bootstrap.pypa.io/get-pip.py -o get-pip.py
python3 get-pip.py
echo 'export PATH=$HOME/.local/bin;$PATH' >> ~/.bashrc
. ~/.bashrc
sudo apt-get install libpython2-dev
```

```bash
pip3 install mkdocs
pip3 install pymdown-extensions
pip3 install mkdocs-material
```

To start the local webserver:

```bash
mkdocs serve
```

To view the docs, open the URL: http://127.0.0.1:8000/

