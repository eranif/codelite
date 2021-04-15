## To build the docs:

Install the following

- python3
- pip3
- install mkdocs + extensions
- If you are under MSYS2, make sure to run: `pacman -S libcrypt-devel`

Install `pip3`:

```bash
# make sure we got curl installed
sudo apt-get install curl
# install pip3
curl https://bootstrap.pypa.io/get-pip.py -o get-pip.py
python3 get-pip.py
sudo apt-get install libpython-dev
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

