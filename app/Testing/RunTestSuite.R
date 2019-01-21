##############################
# Testing for mdpmf.exe

# Configuration here:

# This specifies from which build directory the tests will be run
build = "Debug"

# Directory where the input, output, and comp files are kept:
inputDir = "TestInputs/"
outputDir = "TestOutputs/"
compDir = "TestComps/"

setwd("C:/Users/MLAMAR/OneDrive - Educational Testing Service/EclipseWorkspace/mdpmf_est/Testing")


###### TESTS ################################################

## GenData Test 01
# Test simulating data with action probabilities
# Use basic Fishing model
TestNum = 1

AET_file = "FishingAE.csv"
SVR_file = "FishingSVR.csv"
RW_file = ""
SimBeta_file = "persBetaData.txt"

genCmd = paste("../",build,"/mdpmf.exe --genData --recActionChoice --actionEffectFile ", inputDir, AET_file, 
			" --rewardFile ", inputDir, SVR_file,
			" --personParamFile ", inputDir, SimBeta_file, 
			" --dataOutputFile ", outputDir,  "GenData_Test", TestNum, ".txt", sep = "")

timestamp()
system(genCmd)
timestamp()

TestNum = TestNum + 1
AET_file = "ExploreAgeMM_AET.csv"
SVR_file = "ExploreAgeMM_SVR.csv"
RW_file = "ExploreAgeMM_RW_AET.csv"
SimBeta_file = "persBetaData.txt"

genCmd = paste("../",build,"/mdpmf.exe --genData  --recActionChoice --actionEffectFile ", inputDir, AET_file, 
			" --worldEffectFile ", inputDir, RW_file, 
			" --rewardFile ", inputDir, SVR_file,
			" --personParamFile ", inputDir, SimBeta_file, 
			" --dataOutputFile ", outputDir,  "GenData_Test", TestNum, ".txt", sep = "")

timestamp()
system(genCmd)
timestamp()

##### ActionChoiceProb Generation
# Just looking at the action choice probabilities for the simulated data.  Does it match?

TestNum = 1
AET_file = "FishingAE.csv"
SVR_file = "FishingSVR.csv"
RW_file = ""
SimBeta_file = "persBetaData.txt"

origOutput = paste(outputDir, "GenData_Test", TestNum, ".txt", sep = "")
newOutput = paste(outputDir, "CalcACP_Test", TestNum, ".txt", sep = "")
file.copy(origOutput, newOutput, overwrite = TRUE)


acpCmd = paste("../",build,"/mdpmf.exe --recActionChoice --actionEffectFile ", inputDir, AET_file, 
			" --rewardFile ", inputDir, SVR_file,
			" --personParamFile ", inputDir, SimBeta_file, 
			" --dataInputFile ", newOutput, 
			sep = "")

timestamp()
system(acpCmd)
timestamp()

TestNum = TestNum + 1
AET_file = "ExploreAgeMM_AET.csv"
SVR_file = "ExploreAgeMM_SVR.csv"
RW_file = "ExploreAgeMM_RW_AET.csv"
SimBeta_file = "persBetaData.txt"

origOutput = paste(outputDir, "GenData_Test", TestNum, ".txt", sep = "")
newOutput = paste(outputDir, "CalcACP_Test", TestNum, ".txt", sep = "")
file.copy(origOutput, newOutput, overwrite = TRUE)


acpCmd = paste("../",build,"/mdpmf.exe --recActionChoice --actionEffectFile ", inputDir, AET_file, 
			" --worldEffectFile ", inputDir, RW_file, 
			" --rewardFile ", inputDir, SVR_file,
			" --personParamFile ", inputDir, SimBeta_file, 
			" --dataInputFile ", newOutput, 
			sep = "")

timestamp()
system(acpCmd)
timestamp()

# Now remove the beliefs and run the acp calc again:

origOutput = paste(outputDir, "GenData_Test", TestNum, ".txt", sep = "")
origDat = read.table(origOutput, header = T)
newDat = origDat[,-(grep("LikesFormal$", colnames(origDat)))]
newOutput = paste(outputDir, "CalcACP_PO_Test", TestNum, ".txt", sep = "")

write.table(newDat, newOutput, row.names = F, quote = F)

acpCmd = paste("../",build,"/mdpmf.exe --recActionChoice --actionEffectFile ", inputDir, AET_file, 
			" --worldEffectFile ", inputDir, RW_file, 
			" --rewardFile ", inputDir, SVR_file,
			" --personParamFile ", inputDir, SimBeta_file, 
			" --dataInputFile ", newOutput, 
			sep = "")

timestamp()
system(acpCmd)
timestamp()

# Check to see how different they are:
acp1= read.table(paste(outputDir, "GenData_Test", TestNum, ".acp", sep = ""), header = T)
acp2= read.table(paste(outputDir, "CalcACP_Test", TestNum, ".acp", sep = ""), header = T)
acp3 = read.table(paste(outputDir, "CalcACP_PO_Test", TestNum, ".acp", sep = ""), header = T)

# These should all be zero:
sum(rowSums(acp1[,3:ncol(acp1)]) < 0.99)
sum(rowSums(acp2[,3:ncol(acp1)]) < 0.99)
sum(rowSums(acp3[,3:ncol(acp1)]) < 0.99)
sum(acp1 != acp2)
sum(acp1[,1:2] != acp3[,1:2])

# We don't expect these to be zero:
sum(acp1[,3:ncol(acp1)] != acp3[,3:ncol(acp1)])
max(abs(acp1[,3:ncol(acp1)] - acp3[,3:ncol(acp1)]))
sum(abs(acp1[,3:ncol(acp1)] - acp3[,3:ncol(acp1)]) > 0.1)
largeDiffProp = sum(abs(acp1[,3:ncol(acp1)] - acp3[,3:ncol(acp1)]) > 0.1)/ (dim(acp1[,3:ncol(acp1)])[1] * dim(acp1[,3:ncol(acp1)])[2])


#######################################################################
## CompileModel Test 02
# Test compiling a model, saving and reading it in again
# Use basic Fishing model

TestNum = 1
AET_file = "FishingAE.csv"
SVR_file = "FishingSVR.csv"
RW_file = ""
SimBeta_file = "persBetaData.txt"

runCmd = paste("../",build,"/mdpmf.exe --actionEffectFile ", inputDir, AET_file, 
			" --rewardFile ", inputDir, SVR_file,
			" --compileModel ",
			sep = "")

timestamp()
system(runCmd)
timestamp()

# Now read in the compiled model
# (right now this is just for debugging ... eventually we'll test that it is equivalent)
cmFile = gsub(".csv", ".cmdp", AET_file)

runCmd = paste("../",build,"/mdpmf.exe ",
			" --compModelFile ", inputDir, cmFile,
			" --genData ",
			" --personParamFile ", inputDir, SimBeta_file, 
			" --dataOutputFile ", outputDir,  "CompMod_Test", TestNum, ".txt",
			sep = "")

timestamp()
system(runCmd)
timestamp()

# Estimate action probs with the model files


origOutput = paste(outputDir, "CompMod_Test", TestNum, ".txt", sep = "")
newOutput = paste(outputDir, "CompMod2_Test", TestNum, ".txt", sep = "")
file.copy(origOutput, newOutput, overwrite = TRUE)

acpCmd = paste("../",build,"/mdpmf.exe --recActionChoice --actionEffectFile ", inputDir, AET_file, 
               " --rewardFile ", inputDir, SVR_file,
               " --personParamFile ", inputDir, SimBeta_file, 
               " --dataInputFile ", origOutput, 
               sep = "")

timestamp()
system(acpCmd)
timestamp()

acpCmd = paste("../",build,"/mdpmf.exe --recActionChoice ",
               " --compModelFile ", inputDir, cmFile,
               " --personParamFile ", inputDir, SimBeta_file, 
               " --dataInputFile ", newOutput, 
               sep = "")

timestamp()
system(acpCmd)
timestamp()


# Check to see how different they are:
acp1= read.table(paste(outputDir, "CompMod_Test", TestNum, ".acp", sep = ""), header = T)
acp2= read.table(paste(outputDir, "CompMod2_Test", TestNum, ".acp", sep = ""), header = T)

# Should be zero:
sum(acp1[,3:ncol(acp1)] != acp2[,3:ncol(acp2)])
