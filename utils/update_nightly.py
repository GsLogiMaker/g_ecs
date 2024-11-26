
import argparse
import shutil
import subprocess
import fnmatch
import os

nightly_ignores = [
	"./cpp/*",
	"./.git/*",
	"./.github/*",
	"./.vscode/*",
	"./unittests/*",
	"./utils/*",
	"./.gitmodules",
	"./.gitignore",
]

def main() -> None:
	p = argparse.ArgumentParser(
    	prog='UpdateNightly',
		description='Updates the nightly branch of Glecs from the dev branch.',
	)
	p.add_argument(
		"--repository",
		type=str,
		required=True,
		help="The owner and name of the repository. (Ex. GsLogiMaker/glecs_godot_plugin)",
	)

	args = p.parse_args()

	os.mkdir("../nightly")
	clone_nightly(args.repository)
	copy_dev_plugin_to_nightly()

def clone_nightly(repo:str) -> None:
	subprocess.run(["git", "clone", f"https://github.com/{repo}", f"../nightly_tmp", "-b", "nightly"])
	subprocess.run(["cp", "-r", f"../nightly_tmp/.git/", f"../nightly/.git/"])

def copy_dev_plugin_to_nightly() -> None:
	# Get all paths
	walker = os.walk("./")
	paths:list[str] = []
	for item in walker:
		for folder in item[1]:
			paths.append(os.path.join(item[0], folder) + "/")
		for file in item[2]:
			paths.append(os.path.join(item[0], file))

	# Filter paths to copy
	filtered_paths = list((n for n in paths
		if not any(
			fnmatch.fnmatch(n, ignore) for ignore in nightly_ignores
		)
	))

	# Create directories
	for path in filtered_paths:
		if not path.endswith("/"):
			continue
		os.mkdir(os.path.join("../nightly/", path))

	# Copy files
	for path in filtered_paths:
		if path.endswith("/"):
			continue
		shutil.copy(path, os.path.join("../nightly/", path))

def commit_to_nightly() -> None:
	cwd = os.getcwd()
	subprocess.run(["cd", "../nightly"])
	subprocess.run(["git", "config", "--global", "user.name", "github-actions[bot]"])
	subprocess.run(["git", "config", "--global", "user.email", "gslogimaker@gmail.com"])
	subprocess.run(["git", "commit", "--all", "-m", "(AUTO) ${{ github.event.head_commit.message }}"])
	subprocess.run(["cd", cwd])


if __name__ == "__main__":
	main()
