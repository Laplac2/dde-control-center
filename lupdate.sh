#!/bin/bash

rm ./translations/dde-control-center.ts

lupdate ./ -ts -no-obsolete dde-control-center.ts

cd ../

tx push -s -b m20
