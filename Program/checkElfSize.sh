size_info=$(arm-none-eabi-size "build/SmartNixieClock.elf" | awk NR\>1); \
text=$(echo "$size_info" | awk '{print $1}'); \
data=$(echo "$size_info" | awk '{print $2}'); \
bss=$(echo "$size_info" | awk '{print $3}'); \
flash=$(($text + $data)); \
sram=$(($bss + $data)); \
flashP=$((100*$flash/64000)); \
sramP=$((100*$sram/20000)); \
totalFlash=64000;\
totalsram=20000;\
echo "STM32F103C8T6";\
echo "FLASH used                    = $flash bytes / $totalFlash bytes    $flashP %"; \
echo "SRAM used by global variables = $sram bytes / $totalsram bytes     $sramP %"