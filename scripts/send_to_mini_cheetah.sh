#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

cd ${DIR}/../mc-build/
rm -rf robot-software
mkdir robot-software
mkdir robot-software/build
#cp common/test-common robot-software/build
cp $1 robot-software/build
cp ../third-party/XsensImu/lib/*.* robot-software/build
find . -name \*.so* -exec cp {} ./robot-software/build \;
cp ../scripts/run_mc.sh ./robot-software/build
cp ../scripts/run_mc2.sh ./robot-software/build
cp ../scripts/run_mc_auto.sh ./robot-software/build
cp ../scripts/_PWD_TEMP_ ./robot-software/build
cp ../scripts/run_mc_debug.sh ./robot-software/build
cp ../scripts/config_network_lcm.sh ./robot-software
cp -r ../robot robot-software
cp -r ../config robot-software

DATE=$(date +"%Y%m%d%H%M")
#scp -r robot-software user@10.0.0.34:~/robot-software-$DATE/
#scp -r robot-software dd@192.168.1.104:~/
#scp -r robot-software chee@10.0.0.34:~/
#scp -r robot-software bp5@10.0.0.20:~/
scp -r robot-software upboard@10.0.0.33:~/
# rsync -avz robot-software bp5@10.0.0.20:~/
#scp -r robot-software bipe@10.0.0.20:~/
#scp -r robot-software pi@10.0.0.30:~/
#scp -r robot-software bug@10.0.0.35:~/