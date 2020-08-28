################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
fatfs/doc/res/%.obj: ../fatfs/doc/res/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs1010/ccs/tools/compiler/ti-cgt-arm_20.2.1.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --include_path="C:/Users/bassg/workspace_v10/uhf_base" --include_path="C:/Users/bassg/workspace_v10/uhf_base/SDCard" --include_path="C:/Users/bassg/workspace_v10/uhf_base/fatfs/src" --include_path="C:/Users/bassg/workspace_v10/uhf_base/fatfs/port" --include_path="C:/Users/bassg/workspace_v10/uhf_base/include" --include_path="C:/Users/bassg/workspace_v10/uhf_base/include/dfgm" --include_path="C:/ti/ccs1010/ccs/tools/compiler/ti-cgt-arm_20.2.1.LTS/include" --include_path="C:/Users/bassg/workspace_v10/uhf_base/source/dfgm" -g --printf_support=full --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="fatfs/doc/res/$(basename $(<F)).d_raw" --obj_directory="fatfs/doc/res" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


