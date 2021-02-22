#!/bin/sh

cd "`dirname "$0"`"

# Read version number
read -p "Release version: " version

git tag "$version"

# Create tarball
tarball="AliceTPCTracker-$version.tar.gz"
git archive --format=tar --prefix="AliceTPCTracker-$version/" "$version" | gzip > ../"$tarball"

wwwdir="$HOME/public_html/Tracker"

if test -d "$wwwdir" -a ! -e "$wwwdir/$tarball"; then
	if echo "$version"|grep -q 'unrestricted'; then
		cp ../"$tarball" "$wwwdir/$tarball"
		sed -i "s#//MARKER#//MARKER\n\t\"$version\",#" "$HOME/public_html/Tracker/index.php"
	fi
fi
