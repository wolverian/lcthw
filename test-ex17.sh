#!/bin/sh

set -eux

p=./ex17
db=`mktemp ex17.db.XXX`

$p $db c 1024 512
$p $db l
$p $db s 1 joe joe@danger.com
$p $db l

rm -f $db
