################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
_3_Board/source/%.obj: ../_3_Board/source/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs920/ccs/tools/compiler/ti-cgt-c2000_18.12.3.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu0 --include_path="D:/WorkSpace CCS/DetaiPV/DC_AC_SinglePhase/PV_DC_AC_1PHASE_20_03_5khz/_1_PV_DC_AC/include" --include_path="C:/ti/ccs920/ccs/tools/compiler/ti-cgt-c2000_18.12.3.LTS/include" --include_path="D:/WorkSpace CCS/DetaiPV/DC_AC_SinglePhase/PV_DC_AC_1PHASE_20_03_5khz/_2_Libruary/App_lib/Controller/include" --include_path="D:/WorkSpace CCS/DetaiPV/DC_AC_SinglePhase/PV_DC_AC_1PHASE_20_03_5khz/_2_Libruary/App_lib/Motor/include" --include_path="D:/WorkSpace CCS/DetaiPV/DC_AC_SinglePhase/PV_DC_AC_1PHASE_20_03_5khz/_2_Libruary/App_lib/MPPT/include" --include_path="D:/WorkSpace CCS/DetaiPV/DC_AC_SinglePhase/PV_DC_AC_1PHASE_20_03_5khz/_2_Libruary/App_lib/Observer/include" --include_path="D:/WorkSpace CCS/DetaiPV/DC_AC_SinglePhase/PV_DC_AC_1PHASE_20_03_5khz/_2_Libruary/App_lib/Other/include" --include_path="D:/WorkSpace CCS/DetaiPV/DC_AC_SinglePhase/PV_DC_AC_1PHASE_20_03_5khz/_2_Libruary/App_lib/Phaselockedloop/include" --include_path="D:/WorkSpace CCS/DetaiPV/DC_AC_SinglePhase/PV_DC_AC_1PHASE_20_03_5khz/_2_Libruary/App_lib/PWM/include" --include_path="D:/WorkSpace CCS/DetaiPV/DC_AC_SinglePhase/PV_DC_AC_1PHASE_20_03_5khz/_2_Libruary/App_lib/Transform/include" --include_path="D:/WorkSpace CCS/DetaiPV/DC_AC_SinglePhase/PV_DC_AC_1PHASE_20_03_5khz/_2_Libruary/Math/IQmath/include" --include_path="D:/WorkSpace CCS/DetaiPV/DC_AC_SinglePhase/PV_DC_AC_1PHASE_20_03_5khz/_2_Libruary/Math/CLAmath/include" --include_path="D:/WorkSpace CCS/DetaiPV/DC_AC_SinglePhase/PV_DC_AC_1PHASE_20_03_5khz/_2_Libruary/Driver/include" --include_path="D:/WorkSpace CCS/DetaiPV/DC_AC_SinglePhase/PV_DC_AC_1PHASE_20_03_5khz/_3_Board/include" --include_path="D:/WorkSpace CCS/DetaiPV/DC_AC_SinglePhase/PV_DC_AC_1PHASE_20_03_5khz/_4_Device/F2837xD_common/inc" --include_path="D:/WorkSpace CCS/DetaiPV/DC_AC_SinglePhase/PV_DC_AC_1PHASE_20_03_5khz/_4_Device/F2837xD_headers/include" --include_path="D:/WorkSpace CCS/DetaiPV/DC_AC_SinglePhase/PV_DC_AC_1PHASE_20_03_5khz/_4_Device/F2837xD_common/include" --include_path="D:/WorkSpace CCS/DetaiPV/DC_AC_SinglePhase/PV_DC_AC_1PHASE_20_03_5khz/_4_Device/F2837xD_common/driverlib" --advice:performance=all --define=CPU1 --define=RAM -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="_3_Board/source/$(basename $(<F)).d_raw" --obj_directory="_3_Board/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


