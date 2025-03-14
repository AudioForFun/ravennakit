import argparse
import glob
import subprocess
from pathlib import Path

import pygit2

# Git version
repo = pygit2.Repository(path='.')
git_version = repo.describe(pattern='v*')
git_branch = repo.head.shorthand

script_path = Path(__file__)
script_dir = script_path.parent


def upload_docs_using_sftp(args):
    files = glob.glob(str(args.path_to_build) + '/*-docs.zip')
    if files is None:
        print("No files found")
        return

    tmp_file_name = f'docs_{git_version}.zip'

    scp_cmd = ['scp', '-P', f'{args.ssh_port}', '-r', files[0],
               f'{args.username}@{args.hostname}:{args.remote_path}/{tmp_file_name}']
    ssh_cmd = ['ssh', f'{args.username}@{args.hostname}', '-p', args.ssh_port,
               f'mkdir -p {args.remote_path}/{git_version} && unzip {args.remote_path}/{tmp_file_name} -d {args.remote_path}/{git_version} && rm {args.remote_path}/{tmp_file_name}']

    subprocess.run(scp_cmd, check=True)
    subprocess.run(ssh_cmd, check=True)


def main():
    parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument("--path-to-build", type=Path, help="Path to the build folder", default='build')
    parser.add_argument("--hostname", type=str, help="SSH hostname", required=True)
    parser.add_argument("--username", type=str, help="SSH username", required=True)
    parser.add_argument("--ssh-port", type=str, help="SSH port", default=22)
    parser.add_argument("--remote-path",
                        type=str,
                        help="The path on the remote server to upload the files to",
                        required=True)

    upload_docs_using_sftp(parser.parse_args())


if __name__ == '__main__':
    print("Invoke {} as script. Script dir: {}".format(script_path, script_dir))
    main()
