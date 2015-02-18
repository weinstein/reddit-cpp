#include "file_utils.h"
using namespace reddit;

#include <gtest/gtest.h>
#include <string>
using std::string;

const char kHelloFile[] = "testing/hello.txt";
const char kHelloWorld[] = "Hello, world!\n";
const char kDoesntExist[] = "testing/doesnt.exist";

const char kTempFile[] = "testing/tmp.txt";

TEST(TestFileUtils, FileExists) {
   EXPECT_TRUE(DoesFileExist(kHelloFile));
   EXPECT_FALSE(DoesFileExist(kDoesntExist));
}

TEST(TestFileUtils, ReadFileToString) {
   string hello_world = ReadFileToString(kHelloFile);
   EXPECT_EQ(hello_world, kHelloWorld);
}

TEST(TestFileUtils, WriteStringToFile) {
   EXPECT_FALSE(DoesFileExist(kTempFile));
   WriteStringToFile(kTempFile, kHelloWorld);

   string contents = ReadFileToString(kTempFile);
   EXPECT_EQ(contents, kHelloWorld);

   AppendStringToFile(kTempFile, kHelloWorld);
   contents = ReadFileToString(kTempFile);
   EXPECT_EQ(contents, string(kHelloWorld) + string(kHelloWorld));

   DeleteFile(kTempFile);
   EXPECT_FALSE(DoesFileExist(kTempFile));
}
