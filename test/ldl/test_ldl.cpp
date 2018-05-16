/*
 *  Copyright (C) 2015 Simon Boyé
 *
 *  This file is part of lair.
 *
 *  lair is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  lair is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with lair.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <gtest/gtest.h>

#include <lair/ldl/ldl.h>


using namespace lair;


void checkErrors(ErrorList& errors, int expErrors = 0, int expWarnings = 0) {
	int nErrors   = 0;
	int nWarnings = 0;
	for(unsigned i = 0; i < errors.nErrors(); ++i) {
		if(errors.errorType(i) == ErrorList::ERROR)
			++nErrors;
		else
			++nWarnings;
		std::cerr << "Parse error: " << errors.errorMessage(i) << "\n";
	}
	ASSERT_EQ(expErrors,   nErrors);
	ASSERT_EQ(expWarnings, nWarnings);
	errors.clear();
}

TEST(TestLdl, BasicTypes) {
	const char* test = "null\n"
	                   "false, true\n"
	                   "-123, 0123, 0x123, 0o123, 0b101010\n"
	                   "1.5, 5., .5, 5e5, -5.5e-5\n"
	                   "ident, \"foo's\", '\"hello\"', \"\"\"test ' \" ''' test\"\"\", '''test ' \" \"\"\" test'''\n";

	ErrorList errors;
	std::istringstream in(test);
	LdlParser parser(&in, "<test>", &errors, LdlParser::CTX_LIST);

	checkErrors(errors);
	parser.enter();

	checkErrors(errors);
	ASSERT_EQ(LdlParser::TYPE_NULL, parser.valueType());
//	ASSERT_EQ(1, parser.line());
//	ASSERT_EQ(1, parser.col());

	parser.next();
	checkErrors(errors);
	ASSERT_EQ(LdlParser::TYPE_BOOL, parser.valueType());
	ASSERT_FALSE(parser.getBool());
	parser.next();
	checkErrors(errors);
	ASSERT_EQ(LdlParser::TYPE_BOOL, parser.valueType());
	ASSERT_TRUE(parser.getBool());

	parser.next();
	checkErrors(errors);
	ASSERT_EQ(LdlParser::TYPE_INT, parser.valueType());
	ASSERT_EQ(-123, parser.getInt());
	parser.next();
	checkErrors(errors);
	ASSERT_EQ(LdlParser::TYPE_INT, parser.valueType());
	ASSERT_EQ(123, parser.getInt());
	parser.next();
	checkErrors(errors);
	ASSERT_EQ(LdlParser::TYPE_INT, parser.valueType());
	ASSERT_EQ(0x123, parser.getInt());
	parser.next();
	checkErrors(errors);
	ASSERT_EQ(LdlParser::TYPE_INT, parser.valueType());
	ASSERT_EQ(0123, parser.getInt());
	parser.next();
	checkErrors(errors);
	ASSERT_EQ(LdlParser::TYPE_INT, parser.valueType());
	ASSERT_EQ(0x2a, parser.getInt());

	parser.next();
	checkErrors(errors);
	ASSERT_EQ(LdlParser::TYPE_FLOAT, parser.valueType());
	ASSERT_EQ(1.5, parser.getFloat());
	parser.next();
	checkErrors(errors);
	ASSERT_EQ(LdlParser::TYPE_FLOAT, parser.valueType());
	ASSERT_EQ(5., parser.getFloat());
	parser.next();
	checkErrors(errors);
	ASSERT_EQ(LdlParser::TYPE_FLOAT, parser.valueType());
	ASSERT_EQ(.5, parser.getFloat());
	parser.next();
	checkErrors(errors);
	ASSERT_EQ(LdlParser::TYPE_FLOAT, parser.valueType());
	ASSERT_EQ(5e5, parser.getFloat());
	parser.next();
	checkErrors(errors);
	ASSERT_EQ(LdlParser::TYPE_FLOAT, parser.valueType());
	ASSERT_EQ(-5.5e-5, parser.getFloat());

	parser.next();
	checkErrors(errors);
	ASSERT_EQ(LdlParser::TYPE_STRING, parser.valueType());
	ASSERT_EQ("ident", parser.getString());
	parser.next();
	checkErrors(errors);
	ASSERT_EQ(LdlParser::TYPE_STRING, parser.valueType());
	ASSERT_EQ("foo's", parser.getString());
	parser.next();
	checkErrors(errors);
	ASSERT_EQ(LdlParser::TYPE_STRING, parser.valueType());
	ASSERT_EQ("\"hello\"", parser.getString());
	parser.next();
	checkErrors(errors);
	ASSERT_EQ(LdlParser::TYPE_STRING, parser.valueType());
	ASSERT_EQ("test ' \" ''' test", parser.getString());
	parser.next();
	checkErrors(errors);
	ASSERT_EQ(LdlParser::TYPE_STRING, parser.valueType());
	ASSERT_EQ("test ' \" \"\"\" test", parser.getString());

	parser.next();
	checkErrors(errors);
	ASSERT_EQ(LdlParser::TYPE_END, parser.valueType());

	parser.leave();
	checkErrors(errors);
}

TEST(TestLdl, EscapeSequences) {
	const char* ldl = u8"\"\\\n \\\\ \\' \\\" \\a \\b \\f \\n \\r \\t \\v \\0 \\1234 \\x24 \\u20ac \\U00010348 .$.¢.€.𐍈.\"";
	const char* expectedRaw = u8" \\ \' \" \a \b \f \n \r \t \v \0 \1234 \x24 \u20ac \U00010348 .$.¢.€.𐍈.";
	String expected(expectedRaw, sizeof(u8" \\ \' \" \a \b \f \n \r \t \v \0 \1234 \x24 \u20ac \U00010348 .$.¢.€.𐍈.") - 1);

	ErrorList errors;
	std::istringstream in(ldl);
	LdlParser parser(&in, "<test>", &errors, LdlParser::CTX_LIST);

	checkErrors(errors);
	parser.enter();

	checkErrors(errors);
	ASSERT_EQ(LdlParser::TYPE_STRING, parser.valueType());
	ASSERT_EQ(expected, parser.getString());
	parser.next();

	parser.leave();
	checkErrors(errors);
}

TEST(TestLdl, CompoundTypes) {
	const char* test = "a = 1, b = 2, c =[\n"
	                   "  0, 1, 2 ], d = foo(0, 1, 2), e = {\n"
	                   "  a = 0, b = 1, c = 2 }";

	ErrorList errors;
	std::istringstream in(test);
	LdlParser parser(&in, "<test>", &errors, LdlParser::CTX_MAP);

	checkErrors(errors);
	parser.enter();

	checkErrors(errors);
	ASSERT_EQ("a", parser.getKey());
	ASSERT_EQ(LdlParser::TYPE_INT, parser.valueType());
	ASSERT_EQ(1, parser.getInt());
	parser.next();
	checkErrors(errors);
	ASSERT_EQ("b", parser.getKey());
	ASSERT_EQ(LdlParser::TYPE_INT, parser.valueType());
	ASSERT_EQ(2, parser.getInt());

	parser.next();
	checkErrors(errors);
	ASSERT_EQ("c", parser.getKey());
	ASSERT_EQ(LdlParser::TYPE_LIST, parser.valueType());
	ASSERT_FALSE(parser.isValueTyped());
	parser.enter();
	checkErrors(errors);
	ASSERT_EQ(LdlParser::TYPE_INT, parser.valueType());
	ASSERT_EQ(0, parser.getInt());
	parser.next();
	checkErrors(errors);
	ASSERT_EQ(LdlParser::TYPE_INT, parser.valueType());
	ASSERT_EQ(1, parser.getInt());
	parser.next();
	checkErrors(errors);
	ASSERT_EQ(LdlParser::TYPE_INT, parser.valueType());
	ASSERT_EQ(2, parser.getInt());
	parser.next();
	checkErrors(errors);
	ASSERT_EQ(LdlParser::TYPE_END, parser.valueType());
	parser.leave();

	checkErrors(errors);
	ASSERT_EQ("d", parser.getKey());
	ASSERT_EQ(LdlParser::TYPE_LIST, parser.valueType());
	ASSERT_TRUE(parser.isValueTyped());
	ASSERT_EQ("foo", parser.getValueTypeName());
	parser.enter();
	checkErrors(errors);
	ASSERT_EQ(LdlParser::TYPE_INT, parser.valueType());
	ASSERT_EQ(0, parser.getInt());
	parser.next();
	checkErrors(errors);
	ASSERT_EQ(LdlParser::TYPE_INT, parser.valueType());
	ASSERT_EQ(1, parser.getInt());
	parser.next();
	checkErrors(errors);
	ASSERT_EQ(LdlParser::TYPE_INT, parser.valueType());
	ASSERT_EQ(2, parser.getInt());
	parser.next();
	checkErrors(errors);
	ASSERT_EQ(LdlParser::TYPE_END, parser.valueType());
	parser.leave();

	checkErrors(errors);
	ASSERT_EQ("e", parser.getKey());
	ASSERT_EQ(LdlParser::TYPE_MAP, parser.valueType());
	ASSERT_FALSE(parser.isValueTyped());
	parser.enter();
	checkErrors(errors);
	ASSERT_EQ("a", parser.getKey());
	ASSERT_EQ(LdlParser::TYPE_INT, parser.valueType());
	ASSERT_EQ(0, parser.getInt());
	parser.next();
	checkErrors(errors);
	ASSERT_EQ("b", parser.getKey());
	ASSERT_EQ(LdlParser::TYPE_INT, parser.valueType());
	ASSERT_EQ(1, parser.getInt());
	parser.next();
	checkErrors(errors);
	ASSERT_EQ("c", parser.getKey());
	ASSERT_EQ(LdlParser::TYPE_INT, parser.valueType());
	ASSERT_EQ(2, parser.getInt());
	parser.next();
	checkErrors(errors);
	ASSERT_EQ(LdlParser::TYPE_END, parser.valueType());
	parser.leave();

	ASSERT_EQ(LdlParser::TYPE_END, parser.valueType());

	parser.leave();
	checkErrors(errors);
}

TEST(TestLdl, WriteLdl) {
//	std::ostringstream out;
	LdlWriter writer(&std::cerr, "<test>", nullptr);

	writer.openList();

	writer.writeNull();
	writer.writeBool(true);
	writer.writeBool(false);
	writer.writeInt(42);
	writer.writeInt(42, LdlWriter::IF_BINARY);
	writer.writeInt(42, LdlWriter::IF_OCTAL);
	writer.writeInt(42, LdlWriter::IF_HEX);
	writer.writeFloat(123.456);
	writer.writeString("hello \"world\"");
	writer.writeString("test ' \" ''' \"\"\"", LdlWriter::SF_SINGLE);
	writer.writeString("test ' \" ''' \"\"\"", LdlWriter::SF_DOUBLE);
	writer.writeString("test ' \" ''' \"\"\"", LdlWriter::SF_TRIPLE_SINGLE);
	writer.writeString("test ' \" ''' \"\"\"", LdlWriter::SF_TRIPLE_DOUBLE);
	writer.writeString("\1 \2 \3");
	writer.writeInt(1);
	writer.writeInt(123);

	writer.openList();
	writer.writeInt(0);
	writer.writeInt(1);
	writer.writeInt(2);
	writer.close();

	writer.openList(LdlWriter::CF_SINGLE_LINE, "bar");
	writer.writeInt(0);
	writer.writeInt(1);
	writer.writeInt(2);
	writer.close();

	writer.openMap(LdlWriter::CF_SINGLE_LINE, "baz");
	writer.writeKey("a");
	writer.writeInt(0);
	writer.writeKey("b");
	writer.writeInt(1);
	writer.writeKey("c");
	writer.writeInt(2);
	writer.close();

	writer.close();

	std::cerr << "\n";
}

TEST(TestLdl, ReadMatrix) {
	const char* test = "Vector(0, 1, 2, 3)\n"
	                   "Vector(0, 1)\n"
	                   "Matrix([0, 1], [2, 3])\n"
	                   "Matrix([0, 1, 2, 3], [4, 5, 6, 7], [8, 9, 10, 11])\n";

	ErrorList errors;
	std::istringstream in(test);
	LdlParser parser(&in, "<test>", &errors, LdlParser::CTX_LIST);

	checkErrors(errors);
	parser.enter();
	checkErrors(errors);

	Vector4 vec;
	ASSERT_TRUE(ldlRead(parser, vec));
	checkErrors(errors);
	ASSERT_EQ(Vector4(0, 1, 2, 3), vec);
	ASSERT_TRUE(ldlRead(parser, vec));
	checkErrors(errors);
	ASSERT_EQ(Vector4(0, 1, 0, 1), vec);

	Matrix4 mat;
	Matrix4 mat0 = Matrix4::Identity();
	mat0.topLeftCorner<2, 2>() << 0, 1, 2, 3;
	Matrix4 mat1 = Matrix4::Identity();
	mat1 << 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0, 0, 0, 1;

	ASSERT_TRUE(ldlRead(parser, mat));
	checkErrors(errors);
	ASSERT_EQ(mat0, mat);
	ASSERT_TRUE(ldlRead(parser, mat));
	checkErrors(errors);
	ASSERT_EQ(mat1, mat);

	parser.leave();
	checkErrors(errors);
}

TEST(TestLdl, ReadBox) {
	const char* test = "null\n"
	                   "Box{ min = Vector(1, 2), max = Vector(3, 4) }\n"
	                   "Box{ min = Vector(1, 2), size = Vector(2, 2) }\n"
	                   "Box{ min = Vector(3, 4), max = Vector(1, 2) }\n";

	ErrorList errors;
	std::istringstream in(test);
	LdlParser parser(&in, "<test>", &errors, LdlParser::CTX_LIST);

	checkErrors(errors);
	parser.enter();
	checkErrors(errors);

	Box2 box;

	ASSERT_TRUE(ldlRead(parser, box));
	checkErrors(errors);
	ASSERT_TRUE(box.isEmpty());

	ASSERT_TRUE(ldlRead(parser, box));
	checkErrors(errors);
	ASSERT_TRUE(Box2(Vector2(1, 2), Vector2(3, 4)).isApprox(box));

	ASSERT_TRUE(ldlRead(parser, box));
	checkErrors(errors);
	ASSERT_TRUE(Box2(Vector2(1, 2), Vector2(3, 4)).isApprox(box));

	ASSERT_TRUE(ldlRead(parser, box));
	checkErrors(errors, 0, 1);
	ASSERT_TRUE(box.isEmpty());

	parser.leave();
	checkErrors(errors);
}
