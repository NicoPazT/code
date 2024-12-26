#!/bin/bash

ncores=2

seedList="0.234 0.345"

parallel -j ${ncores} bash tests.sh {} ::: ${seedList}