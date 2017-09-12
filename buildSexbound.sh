#!/bin/sh

cd ..

BASE=$(pwd)
DATA_DIR="$BASE/mods/SexboundAPI_Data"
DATA_BUILD_DIR="$DATA_DIR/build"
PLAYER_DATA="$BASE/storage/player"
PLAYER_IDENTITIES_FILE="$DATA_DIR/playeridentities.json"

echo $DATA_DIR
echo $DATA_BUILD_DIR
echo $PLAYER_DATA

#Create dir of not exist
mkdir -p $DATA_DIR
mkdir -p $DATA_BUILD_DIR

#clean identities file
echo "" > $PLAYER_IDENTITIES_FILE
for file in $PLAYER_DATA/*.player
do
	file=${file##*/}
  PLAYER_DATA_FILE="${PLAYER_DATA}/${file}"
	PLAYER_DATA_DUMP_JSON="${DATA_BUILD_DIR}/${file}.json"

	$("$BASE/linux/dump_versioned_json" $PLAYER_DATA_FILE $PLAYER_DATA_DUMP_JSON)

	$(cat $PLAYER_DATA_DUMP_JSON | jq ' .content | {(.uuid) : .identity} ' >> $PLAYER_IDENTITIES_FILE)
	echo "," >> $PLAYER_IDENTITIES_FILE
done

#remove last coma
sed -i '$ s/.$//' $PLAYER_IDENTITIES_FILE

echo "Build finished... Have a nice game"
