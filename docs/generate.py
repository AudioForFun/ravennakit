# Script location matters, cwd does not
import subprocess
from pathlib import Path

script_path = Path(__file__)
script_dir = script_path.parent


def doxygen_docs():
    # Generate html docs
    subprocess.run(['doxygen', 'Doxyfile'], cwd=script_dir, check=True)


if __name__ == '__main__':
    print("Invoke {} as script. Script dir: {}".format(script_path, script_dir))
    doxygen_docs()
