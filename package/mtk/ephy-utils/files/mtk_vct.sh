# SPDX-License-Identifier: GPL-2.0

# This script is used as pure VCT(virtual cable test), which means
# it only detects cable's real status no matter which link partner
# is connected. (or even not connected)

# Execute the following if you want simpler output
# dmesg -n1
# mtk_vct.sh -s -p=0x2 | grep "Pair" | sed 's/Pair[A-D]: //'
# mtk_vct.sh -p=0xf | grep "Pair" | sed 's/Pair[A-D]: //'

source lib.sh

total_round=10
decimal_port=$(printf "%d" ${port})

if [ "${TEST_CMD}" == "switch" ]; then
	if [ ${decimal_port} -lt 0 ] || [ ${decimal_port} -gt 5 ]; then
		echo "Please enter port=0~5"
		exit 1
	fi

	if="lan${decimal_port}"
fi

if [ "${TEST_CMD}" != "switch" ]; then
	TEST_CMD="mii"
	if [ ${decimal_port} -eq 0 ]; then
		if="eth0" # mt7981 built-in Gphy
	elif [ ${decimal_port} -eq 15 ]; then
		if="eth1" # mt7988/mt7987 build-in 2.5G phy
	fi
fi

ifconfig ${if} down
echo "Wait a minute if disconnection takes time"
cmd_gen 22 "write" 0x0 0x1040 && ${CMD} > /dev/null
sleep 1

round=1
while [ ! ${round} -gt ${total_round} ]
do
	echo "######## VCT test round${round} ########"
	if [ "${TEST_CMD}" == "switch" ]; then
		/usr/sbin/mtk_vct -s -p ${port}
		#/usr/sbin/mtk_vct -s -p ${port} | \
		#grep -E "Pair[A-D]" | sed 's/Pair[A-D]: //g' | \
		#sed 's/ length=//g' | sed 's/\([0-9]*\+\.[0-9]*\+\)m/\1/g'
	elif [ "${TEST_CMD}" == "mii" ]; then
		/usr/sbin/mtk_vct -p ${port}
		#/usr/sbin/mtk_vct -p ${port} | \
		#grep -E "Pair[A-D]" | sed 's/Pair[A-D]: //g' | \
		#sed 's/ length=//g' | sed 's/\([0-9]*\+\.[0-9]*\+\)m/\1/g'
	fi
	let "round++"
done

ifconfig ${if} up
