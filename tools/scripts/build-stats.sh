#!/bin/bash

LOG_FILE=FILE_SIZE_STATISTICS.TXT
TIME_CMD='/usr/bin/time --format %e,%U,%S'
BUILD_DIR="build"
CORE_LIB="libcore.so"
GAME_LIB="libgame.so"
LAUNCHER="mmrun"
CORE_LIB_STRIPPED="libcore-stripped.so"
GAME_LIB_STRIPPED="libgame-stripped.so"

./waf distclean
STATS_CORE=`wc --lines --chars \`find ./core \( -name *.cpp -or -name *.h \) -and -not \( -path */*/protobuf/* \)\` | tail -1 | awk '{ print $1 }'`
#STATS_CORE=`wc --lines --chars \`find ./core -name "*.cpp" -or -name "*.h"\` | tail -1 | awk '{ print $1 }'`
#STATS_GAME=`wc --lines --chars \`find ./game -name "*.cpp" -or -name "*.h"\` | tail -1 | awk '{ print $1 }'`
STATS_GAME=`wc --lines --chars \`find ./game -name "*.cpp" -or -name "*.h"\` | tail -1 | awk '{ print $1 }'`

# build debug version, then check file sizes
./waf configure -t debug
$TIME_CMD -o DEBUG_BUILD_TIME ./waf
declare -ri CORE_DEBUG_SIZE=`ls -l ${BUILD_DIR}/${CORE_LIB} | cut --delimiter=' ' --fields=5`
declare -ri GAME_DEBUG_SIZE=`ls -l ${BUILD_DIR}/${GAME_LIB} | cut --delimiter=' ' --fields=5`

# build release version, then check file sizes
./waf configure -t release
$TIME_CMD -o RELEASE_BUILD_TIME ./waf
#RELEASE_BUILD_TIME=$($TIME_CMD ./waf)
declare -ri CORE_RELEASE_SIZE=`ls -l ${BUILD_DIR}/${CORE_LIB} | cut --delimiter=' ' --fields=5`
declare -ri GAME_RELEASE_SIZE=`ls -l ${BUILD_DIR}/${GAME_LIB} | cut --delimiter=' ' --fields=5`

# strip symbols from libraries
objcopy -S "${BUILD_DIR}/${CORE_LIB}" "${BUILD_DIR}/${CORE_LIB_STRIPPED}"
objcopy -S "${BUILD_DIR}/${GAME_LIB}" "${BUILD_DIR}/${GAME_LIB_STRIPPED}"
# check file sizes
declare -ri CORE_RELEASE_STRIPPED_SIZE=`ls -l ${BUILD_DIR}/${CORE_LIB_STRIPPED} | cut --delimiter=' ' --fields=5`
declare -ri GAME_RELEASE_STRIPPED_SIZE=`ls -l ${BUILD_DIR}/${GAME_LIB_STRIPPED} | cut --delimiter=' ' --fields=5`

echo "CORE_DEBUG_SIZE            $CORE_DEBUG_SIZE"
echo "GAME_DEBUG_SIZE            $GAME_DEBUG_SIZE"
echo "CORE_RELEASE_SIZE          $CORE_RELEASE_SIZE"
echo "GAME_RELEASE_SIZE          $GAME_RELEASE_SIZE"
echo "CORE_RELEASE_STRIPPED_SIZE $CORE_RELEASE_STRIPPED_SIZE"
echo "GAME_RELEASE_STRIPPED_SIZE $GAME_RELEASE_STRIPPED_SIZE"
echo "DEBUG_BUILD_TIME          " $(cat DEBUG_BUILD_TIME)
echo "RELEASE_BUILD_TIME        " $(cat RELEASE_BUILD_TIME)

HEADER="           datum   core(d)    core(r)   core(rs)    game(d)    game(r)   game(rs)   core(#l)   game(#l)  debug build time / release build time"
DATE=`date "+%d/%m/%Y %H:%M"`

# save results into log file
echo -n $DATE                                   >> $LOG_FILE
printf "%10d "     $CORE_DEBUG_SIZE             >> $LOG_FILE
printf "%10d "     $CORE_RELEASE_SIZE           >> $LOG_FILE
printf "%10d "     $CORE_RELEASE_STRIPPED_SIZE  >> $LOG_FILE
printf "%10d "     $GAME_DEBUG_SIZE             >> $LOG_FILE
printf "%10d "     $GAME_RELEASE_SIZE           >> $LOG_FILE
printf "%10d "     $GAME_RELEASE_STRIPPED_SIZE  >> $LOG_FILE
printf "%10d "     $STATS_CORE                  >> $LOG_FILE
printf "%10d "     $STATS_GAME                  >> $LOG_FILE
printf "%17s"      $(cat DEBUG_BUILD_TIME)      >> $LOG_FILE
printf " / %17s\n" $(cat RELEASE_BUILD_TIME)    >> $LOG_FILE

echo "$HEADER"
tail -5 $LOG_FILE

rm -f DEBUG_BUILD_TIME RELEASE_BUILD_TIME
