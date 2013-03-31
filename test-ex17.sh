#!/bin/sh

set -eux

p=./ex17
db=`mktemp ex17.db.XXX`

clean_up() {
	rm -f $db
}

trap clean_up EXIT

$p $db c 1024 512
$p $db l
$p $db s 1 joe joe@danger.com
$p $db l
