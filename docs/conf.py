from datetime import datetime
from subprocess import call

extensions = ["breathe"]
html_theme = "furo"
html_title = "Logic gates library"

call("doxygen", shell=True)

breathe_projects = {"Logic gates library": "doxygen/xml"}
breathe_default_project = "Logic gates library"

copyright = f'{datetime.now().year} Iteron-dev. Licensed under the GNU GPLv3.'