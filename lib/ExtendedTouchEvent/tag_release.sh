#! /usr/local/bin/bash

set -x
replace_version() {
    mv $1 $1.orig;
    cat $1.orig |sed -E "$2" > $1;
    rm $1.orig;
    git add $1;
}

NEXT_VERSION=.next_version

if [ -a $NEXT_VERSION ]; then
    semvers="^([0-9]+)\.([0-9]+)\.([0-9]+)$";
    [[ `cat $NEXT_VERSION` =~ $semvers ]];
    if [ -n "${BASH_REMATCH[0]}" ]; then
        replace_version library.properties "s/(version=)(.*)$/\1${BASH_REMATCH[0]}/";
        replace_version library.json       "0,/(\"version\":\")(.*)(\".*)$/{s//\1${BASH_REMATCH[0]}\3/}";
        cat /dev/null > $NEXT_VERSION;
        git add $NEXT_VERSION;
        git commit -m "set version to ${BASH_REMATCH[0]}";
        git tag "${BASH_REMATCH[0]}";
        git push origin
        git push origin --tags
    fi
fi

