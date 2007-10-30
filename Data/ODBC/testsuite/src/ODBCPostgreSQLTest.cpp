//
// ODBCPostgreSQLTest.cpp
//
// $Id: //poco/Main/Data/ODBC/testsuite/src/ODBCPostgreSQLTest.cpp#5 $
//
// Copyright (c) 2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "ODBCPostgreSQLTest.h"
#include "CppUnit/TestCaller.h"
#include "CppUnit/TestSuite.h"
#include "Poco/String.h"
#include "Poco/Format.h"
#include "Poco/Tuple.h"
#include "Poco/DateTime.h"
#include "Poco/Exception.h"
#include "Poco/Data/Common.h"
#include "Poco/Data/BLOB.h"
#include "Poco/Data/StatementImpl.h"
#include "Poco/Data/ODBC/Connector.h"
#include "Poco/Data/ODBC/Utility.h"
#include "Poco/Data/ODBC/Diagnostics.h"
#include "Poco/Data/ODBC/ODBCException.h"
#include "Poco/Data/ODBC/ODBCStatementImpl.h"
#include <sqltypes.h>
#include <iostream>


using namespace Poco::Data;
using ODBC::Utility;
using ODBC::ODBCException;
using ODBC::ConnectionException;
using ODBC::StatementException;
using ODBC::StatementDiagnostics;
using Poco::format;
using Poco::Tuple;
using Poco::DateTime;
using Poco::NotFoundException;


ODBCPostgreSQLTest::SessionPtr  ODBCPostgreSQLTest::_pSession = 0;
ODBCPostgreSQLTest::ExecPtr ODBCPostgreSQLTest::_pExecutor = 0;
std::string                     ODBCPostgreSQLTest::_dbConnString;
ODBCPostgreSQLTest::Drivers     ODBCPostgreSQLTest::_drivers;
const bool                      ODBCPostgreSQLTest::bindValues[8] = 
	{true, true, true, false, false, true, false, false};


#ifdef POCO_OS_FAMILY_WINDOWS
const std::string ODBCPostgreSQLTest::libDir = "C:\\\\Program Files\\\\PostgreSQL\\\\8.2\\\\lib\\\\";
#else
const std::string ODBCPostgreSQLTest::libDir = "/usr/local/pgsql/lib/";
#endif


ODBCPostgreSQLTest::ODBCPostgreSQLTest(const std::string& name): 
	CppUnit::TestCase(name)
{
}


ODBCPostgreSQLTest::~ODBCPostgreSQLTest()
{
}


void ODBCPostgreSQLTest::testBareboneODBC()
{
	if (!_pSession) fail ("Test not available.");

	std::string tableCreateString = "CREATE TABLE Test "
		"(First VARCHAR(30),"
		"Second VARCHAR(30),"
		"Third BYTEA,"
		"Fourth INTEGER,"
		"Fifth FLOAT,"
		"Sixth TIMESTAMP)";

	_pExecutor->bareboneODBCTest(_dbConnString, tableCreateString, SQLExecutor::PB_IMMEDIATE, SQLExecutor::DE_MANUAL);
	_pExecutor->bareboneODBCTest(_dbConnString, tableCreateString, SQLExecutor::PB_IMMEDIATE, SQLExecutor::DE_BOUND);
	_pExecutor->bareboneODBCTest(_dbConnString, tableCreateString, SQLExecutor::PB_AT_EXEC, SQLExecutor::DE_MANUAL);
	_pExecutor->bareboneODBCTest(_dbConnString, tableCreateString, SQLExecutor::PB_AT_EXEC, SQLExecutor::DE_BOUND);

	tableCreateString = "CREATE TABLE Test "
		"(First VARCHAR(30),"
		"Second VARCHAR(30),"
		"Third BYTEA,"
		"Fourth INTEGER,"
		"Fifth FLOAT,"
		"Sixth DATE)";

	_pExecutor->bareboneODBCTest(_dbConnString, tableCreateString, SQLExecutor::PB_IMMEDIATE, SQLExecutor::DE_MANUAL, false);
	_pExecutor->bareboneODBCTest(_dbConnString, tableCreateString, SQLExecutor::PB_IMMEDIATE, SQLExecutor::DE_BOUND, false);
	_pExecutor->bareboneODBCTest(_dbConnString, tableCreateString, SQLExecutor::PB_AT_EXEC, SQLExecutor::DE_MANUAL, false);
	_pExecutor->bareboneODBCTest(_dbConnString, tableCreateString, SQLExecutor::PB_AT_EXEC, SQLExecutor::DE_BOUND, false);
}


void ODBCPostgreSQLTest::testSimpleAccess()
{
	if (!_pSession) fail ("Test not available.");

	std::string tableName("Person");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->simpleAccess();
		i += 2;
	}
}


void ODBCPostgreSQLTest::testComplexType()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->complexType();
		i += 2;
	}
}


void ODBCPostgreSQLTest::testSimpleAccessVector()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->simpleAccessVector();
		i += 2;
	}
}


void ODBCPostgreSQLTest::testComplexTypeVector()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->complexTypeVector();
		i += 2;
	}
}


void ODBCPostgreSQLTest::testInsertVector()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateStringsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->insertVector();
		i += 2;
	}	
}


void ODBCPostgreSQLTest::testInsertEmptyVector()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateStringsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->insertEmptyVector();
		i += 2;
	}	
}


void ODBCPostgreSQLTest::testSimpleAccessList()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->simpleAccessList();
		i += 2;
	}
}


void ODBCPostgreSQLTest::testComplexTypeList()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->complexTypeList();
		i += 2;
	}	
}


void ODBCPostgreSQLTest::testInsertList()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateStringsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->insertList();
		i += 2;
	}	
}


void ODBCPostgreSQLTest::testInsertEmptyList()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateStringsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->insertEmptyList();
		i += 2;
	}	
}


void ODBCPostgreSQLTest::testSimpleAccessDeque()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->simpleAccessDeque();
		i += 2;
	}
}


void ODBCPostgreSQLTest::testComplexTypeDeque()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->complexTypeDeque();
		i += 2;
	}	
}


void ODBCPostgreSQLTest::testInsertDeque()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateStringsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->insertDeque();
		i += 2;
	}	
}


void ODBCPostgreSQLTest::testInsertEmptyDeque()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateStringsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->insertEmptyDeque();
		i += 2;
	}	
}


void ODBCPostgreSQLTest::testInsertSingleBulk()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateIntsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->insertSingleBulk();
		i += 2;
	}	
}


void ODBCPostgreSQLTest::testInsertSingleBulkVec()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateIntsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->insertSingleBulkVec();
		i += 2;
	}	
}


void ODBCPostgreSQLTest::testLimit()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateIntsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->limits();
		i += 2;
	}
}


void ODBCPostgreSQLTest::testLimitZero()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateIntsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->limitZero();
		i += 2;
	}	
}


void ODBCPostgreSQLTest::testLimitOnce()
{
	if (!_pSession) fail ("Test not available.");

	recreateIntsTable();
	_pExecutor->limitOnce();
	
}


void ODBCPostgreSQLTest::testLimitPrepare()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateIntsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->limitPrepare();
		i += 2;
	}
}



void ODBCPostgreSQLTest::testPrepare()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateIntsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->prepare();
		i += 2;
	}
}


void ODBCPostgreSQLTest::testSetSimple()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->setSimple();
		i += 2;
	}
}


void ODBCPostgreSQLTest::testSetComplex()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->setComplex();
		i += 2;
	}
}


void ODBCPostgreSQLTest::testSetComplexUnique()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->setComplexUnique();
		i += 2;
	}
}

void ODBCPostgreSQLTest::testMultiSetSimple()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->multiSetSimple();
		i += 2;
	}
}


void ODBCPostgreSQLTest::testMultiSetComplex()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->multiSetComplex();
		i += 2;
	}	
}


void ODBCPostgreSQLTest::testMapComplex()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->mapComplex();
		i += 2;
	}
}


void ODBCPostgreSQLTest::testMapComplexUnique()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->mapComplexUnique();
		i += 2;
	}
}


void ODBCPostgreSQLTest::testMultiMapComplex()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->multiMapComplex();
		i += 2;
	}
}


void ODBCPostgreSQLTest::testSelectIntoSingle()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->selectIntoSingle();
		i += 2;
	}
}


void ODBCPostgreSQLTest::testSelectIntoSingleStep()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->selectIntoSingleStep();
		i += 2;
	}	
}


void ODBCPostgreSQLTest::testSelectIntoSingleFail()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->selectIntoSingleFail();
		i += 2;
	}	
}


void ODBCPostgreSQLTest::testLowerLimitOk()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->lowerLimitOk();
		i += 2;
	}	
}


void ODBCPostgreSQLTest::testSingleSelect()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->singleSelect();
		i += 2;
	}	
}


void ODBCPostgreSQLTest::testLowerLimitFail()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->lowerLimitFail();
		i += 2;
	}
}


void ODBCPostgreSQLTest::testCombinedLimits()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->combinedLimits();
		i += 2;
	}
}



void ODBCPostgreSQLTest::testRange()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->ranges();
		i += 2;
	}
}


void ODBCPostgreSQLTest::testCombinedIllegalLimits()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->combinedIllegalLimits();
		i += 2;
	}
}



void ODBCPostgreSQLTest::testIllegalRange()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->illegalRange();
		i += 2;
	}
}


void ODBCPostgreSQLTest::testEmptyDB()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->emptyDB();
		i += 2;
	}
}


void ODBCPostgreSQLTest::testBLOB()
{
	if (!_pSession) fail ("Test not available.");

	const std::size_t maxFldSize = 1000000;
	_pSession->setProperty("maxFieldSize", Poco::Any(maxFldSize-1));
	recreatePersonBLOBTable();

	try
	{
		_pExecutor->blob(maxFldSize);
		fail ("must fail");
	}
	catch (DataException&) 
	{
		_pSession->setProperty("maxFieldSize", Poco::Any(maxFldSize));
	}

	for (int i = 0; i < 8;)
	{
		recreatePersonBLOBTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->blob(1000000);
		i += 2;
	}
}


void ODBCPostgreSQLTest::testBLOBStmt()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonBLOBTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->blobStmt();
		i += 2;
	}
}


void ODBCPostgreSQLTest::testDateTime()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonDateTimeTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->dateTime();
		i += 2;
	}
}


void ODBCPostgreSQLTest::testFloat()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateFloatsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->floats();
		i += 2;
	}
}


void ODBCPostgreSQLTest::testDouble()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateFloatsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->doubles();
		i += 2;
	}
}


void ODBCPostgreSQLTest::testTuple()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateTuplesTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->tuples();
		i += 2;
	}
}


void ODBCPostgreSQLTest::testTupleVector()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateTuplesTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->tupleVector();
		i += 2;
	}
}


void ODBCPostgreSQLTest::testInternalExtraction()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateVectorsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->internalExtraction();
		i += 2;
	}
}


void ODBCPostgreSQLTest::testInternalStorageType()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateVectorsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->internalStorageType();
		i += 2;
	}
}


void ODBCPostgreSQLTest::testNull()
{
	if (!_pSession) fail ("Test not available.");

	// test for NOT NULL violation exception
	for (int i = 0; i < 8;)
	{
		recreateNullsTable("NOT NULL");
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->notNulls();
		i += 2;
	}

	// test for null insertion
	for (int i = 0; i < 8;)
	{
		recreateNullsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->nulls();
		i += 2;
	}
}


void ODBCPostgreSQLTest::testStoredFunction()
{
	configurePLPgSQL();

	for (int k = 0; k < 8;)
	{
		_pSession->setFeature("autoBind", bindValues[k]);
		_pSession->setFeature("autoExtract", bindValues[k+1]);

		*_pSession << "CREATE FUNCTION storedFunction() RETURNS INTEGER AS '"
			"BEGIN "
			" return -1; "
			"END;'"
			"LANGUAGE 'plpgsql'", now;

		int i = 0;
		*_pSession << "{? = call storedFunction()}", out(i), now;
		assert(-1 == i);
		dropObject("FUNCTION", "storedFunction()");

		*_pSession << "CREATE FUNCTION storedFunction(INTEGER) RETURNS INTEGER AS '"
			"BEGIN "
			" RETURN $1 * $1; "
			"END;'"
			"LANGUAGE 'plpgsql'" , now;

		i = 2;
		int result = 0;
		*_pSession << "{? = call storedFunction(?)}", out(result), in(i), now;
		assert(4 == result);
		dropObject("FUNCTION", "storedFunction(INTEGER)");


		*_pSession << "CREATE FUNCTION storedFunction(TIMESTAMP) RETURNS TIMESTAMP AS '"
			"BEGIN "
			" RETURN $1; "
			"END;'"
			"LANGUAGE 'plpgsql'" , now;

		DateTime dtIn(1965, 6, 18, 5, 35, 1);
		DateTime dtOut;
		*_pSession << "{? = call storedFunction(?)}", out(dtOut), in(dtIn), now;
		assert(dtOut == dtIn);
		dropObject("FUNCTION", "storedFunction(TIMESTAMP)");

		*_pSession << "CREATE FUNCTION storedFunction(TEXT,TEXT) RETURNS TEXT AS '"
			"BEGIN "
			" RETURN $1 || '', '' || $2 || ''!'';"
			"END;'"
			"LANGUAGE 'plpgsql'" , now;
		
		std::string param1 = "Hello";
		std::string param2 = "world";
		std::string ret;
		*_pSession << "{? = call storedFunction(?,?)}", out(ret), in(param1), in(param2), now;
		assert(ret == "Hello, world!");
		dropObject("FUNCTION", "storedFunction(TEXT, TEXT)");

		k += 2;
	}
}


void ODBCPostgreSQLTest::testRowIterator()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateVectorsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->rowIterator();
		i += 2;
	}
}


void ODBCPostgreSQLTest::testStdVectorBool()
{

// psqlODBC driver returns string for bool fields
// even when field is explicitly cast to boolean,
// so this functionality seems to be untestable with it

#ifdef POCO_ODBC_USE_MAMMOTH_NG
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateBoolTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->stdVectorBool();
		i += 2;
	}
#endif // POCO_ODBC_USE_MAMMOTH_NG
}


void ODBCPostgreSQLTest::testAsync()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateIntsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->asynchronous(2000);
		i += 2;
	}
}


void ODBCPostgreSQLTest::configurePLPgSQL()
{
	if (!_pSession) fail ("Test not available.");

	try
	{
		*_pSession << format("CREATE FUNCTION plpgsql_call_handler () "
			"RETURNS OPAQUE "
			"AS '%splpgsql.dll' "
			"LANGUAGE 'C';", libDir), now;
		
		*_pSession << "CREATE LANGUAGE 'plpgsql' "
			"HANDLER plpgsql_call_handler "
			"LANCOMPILER 'PL/pgSQL'", now;

	}catch(StatementException& ex) 
	{  
		if (7 != ex.diagnostics().nativeError(0)) 
			throw;
	}

	return;
}


void ODBCPostgreSQLTest::dropObject(const std::string& type, const std::string& name)
{
	try
	{
		*_pSession << format("DROP %s %s", type, name), now;
	}
	catch (StatementException& ex)
	{
		bool ignoreError = false;
		const StatementDiagnostics::FieldVec& flds = ex.diagnostics().fields();
		StatementDiagnostics::Iterator it = flds.begin();
		for (; it != flds.end(); ++it)
		{
			if (7 == it->_nativeError)//(table does not exist)
			{
				ignoreError = true;
				break;
			}
		}

		if (!ignoreError) throw;
	}
}


void ODBCPostgreSQLTest::recreatePersonTable()
{
	dropObject("TABLE", "Person");
	try { *_pSession << "CREATE TABLE Person (LastName VARCHAR(30), FirstName VARCHAR(30), Address VARCHAR(30), Age INTEGER)", now; }
	catch(ConnectionException& ce){ std::cout << ce.toString() << std::endl; fail ("recreatePersonTable()"); }
	catch(StatementException& se){ std::cout << se.toString() << std::endl; fail ("recreatePersonTable()"); }
}


void ODBCPostgreSQLTest::recreatePersonBLOBTable()
{
	dropObject("TABLE", "Person");
	try { *_pSession << "CREATE TABLE Person (LastName VARCHAR(30), FirstName VARCHAR(30), Address VARCHAR(30), Image BYTEA)", now; }
	catch(ConnectionException& ce){ std::cout << ce.toString() << std::endl; fail ("recreatePersonBLOBTable()"); }
	catch(StatementException& se){ std::cout << se.toString() << std::endl; fail ("recreatePersonBLOBTable()"); }
}



void ODBCPostgreSQLTest::recreatePersonDateTimeTable()
{
	dropObject("TABLE", "Person");
	try { *_pSession << "CREATE TABLE Person (LastName VARCHAR(30), FirstName VARCHAR(30), Address VARCHAR(30), Born TIMESTAMP)", now; }
	catch(ConnectionException& ce){ std::cout << ce.toString() << std::endl; fail ("recreatePersonDateTimeTable()"); }
	catch(StatementException& se){ std::cout << se.toString() << std::endl; fail ("recreatePersonDateTimeTable()"); }
}


void ODBCPostgreSQLTest::recreateIntsTable()
{
	dropObject("TABLE", "Strings");
	try { *_pSession << "CREATE TABLE Strings (str INTEGER)", now; }
	catch(ConnectionException& ce){ std::cout << ce.toString() << std::endl; fail ("recreateIntsTable()"); }
	catch(StatementException& se){ std::cout << se.toString() << std::endl; fail ("recreateIntsTable()"); }
}


void ODBCPostgreSQLTest::recreateStringsTable()
{
	dropObject("TABLE", "Strings");
	try { *_pSession << "CREATE TABLE Strings (str VARCHAR(30))", now; }
	catch(ConnectionException& ce){ std::cout << ce.toString() << std::endl; fail ("recreateStringsTable()"); }
	catch(StatementException& se){ std::cout << se.toString() << std::endl; fail ("recreateStringsTable()"); }
}


void ODBCPostgreSQLTest::recreateFloatsTable()
{
	dropObject("TABLE", "Strings");
	try { *_pSession << "CREATE TABLE Strings (str FLOAT)", now; }
	catch(ConnectionException& ce){ std::cout << ce.toString() << std::endl; fail ("recreateFloatsTable()"); }
	catch(StatementException& se){ std::cout << se.toString() << std::endl; fail ("recreateFloatsTable()"); }
}


void ODBCPostgreSQLTest::recreateTuplesTable()
{
	dropObject("TABLE", "Tuples");
	try { *_pSession << "CREATE TABLE Tuples "
		"(int0 INTEGER, int1 INTEGER, int2 INTEGER, int3 INTEGER, int4 INTEGER, int5 INTEGER, int6 INTEGER, "
		"int7 INTEGER, int8 INTEGER, int9 INTEGER, int10 INTEGER, int11 INTEGER, int12 INTEGER, int13 INTEGER,"
		"int14 INTEGER, int15 INTEGER, int16 INTEGER, int17 INTEGER, int18 INTEGER, int19 INTEGER)", now; }
	catch(ConnectionException& ce){ std::cout << ce.toString() << std::endl; fail ("recreateTuplesTable()"); }
	catch(StatementException& se){ std::cout << se.toString() << std::endl; fail ("recreateTuplesTable()"); }
}


void ODBCPostgreSQLTest::recreateVectorsTable()
{
	dropObject("TABLE", "Vectors");
	try { *_pSession << "CREATE TABLE Vectors (int0 INTEGER, flt0 FLOAT, str0 VARCHAR(30))", now; }
	catch(ConnectionException& ce){ std::cout << ce.toString() << std::endl; fail ("recreateVectorsTable()"); }
	catch(StatementException& se){ std::cout << se.toString() << std::endl; fail ("recreateVectorsTable()"); }
}


void ODBCPostgreSQLTest::recreateNullsTable(const std::string& notNull)
{
	dropObject("TABLE", "NullTest");
	try { *_pSession << format("CREATE TABLE NullTest (i INTEGER %s, r FLOAT %s, v VARCHAR(30) %s)",
		notNull,
		notNull,
		notNull), now; }
	catch(ConnectionException& ce){ std::cout << ce.toString() << std::endl; fail ("recreateNullsTable()"); }
	catch(StatementException& se){ std::cout << se.toString() << std::endl; fail ("recreateNullsTable()"); }
}


void ODBCPostgreSQLTest::recreateBoolTable()
{
	dropObject("TABLE", "BoolTest");
	try { *_pSession << "CREATE TABLE BoolTest (b BOOLEAN)", now; }
	catch(ConnectionException& ce){ std::cout << ce.toString() << std::endl; fail ("recreateBoolTable()"); }
	catch(StatementException& se){ std::cout << se.toString() << std::endl; fail ("recreateBoolTable()"); }
}


bool ODBCPostgreSQLTest::canConnect(const std::string& driver, const std::string& dsn)
{
	Utility::DriverMap::iterator itDrv = _drivers.begin();
	for (; itDrv != _drivers.end(); ++itDrv)
	{
		if (((itDrv->first).find(driver) != std::string::npos))
		{
			std::cout << "Driver found: " << itDrv->first 
				<< " (" << itDrv->second << ')' << std::endl;
			break;
		}
	}

	if (_drivers.end() == itDrv) 
	{
		std::cout << driver << " driver NOT found, tests not available." << std::endl;
		return false;
	}

	Utility::DSNMap dataSources;
	Utility::dataSources(dataSources);
	Utility::DSNMap::iterator itDSN = dataSources.begin();
	for (; itDSN != dataSources.end(); ++itDSN)
	{
		if (itDSN->first == dsn && itDSN->second == driver)
		{
			std::cout << "DSN found: " << itDSN->first 
				<< " (" << itDSN->second << ')' << std::endl;
			format(_dbConnString, "DSN=%s", dsn);
			return true;
		}
	}

	// DSN not found, try connect without it
	format(_dbConnString, "DRIVER=%s;"
		"DATABASE=postgres;"
		"SERVER=localhost;"
		"PORT=5432;"
		"UID=postgres;"
		"PWD=postgres;"
		"SSLMODE=prefer;"
		"LowerCaseIdentifier=0;"
		"UseServerSidePrepare=0;"
		"ByteaAsLongVarBinary=1;"
		"BI=0;"
		"TrueIsMinus1=0;"
		"DisallowPremature=0;"
		"UpdatableCursors=0;"
		"LFConversion=1;"
		"CancelAsFreeStmt=0;"
		"Parse=0;"
		"BoolsAsChar=1;"
		"UnknownsAsLongVarchar=0;"
		"TextAsLongVarchar=1;"
		"UseDeclareFetch=0;"
		"Ksqo=1;"
		"Optimizer=1;"
		"CommLog=0;"
		"Debug=0;"
		"MaxLongVarcharSize=8190;"
		"MaxVarcharSize=254;"
		"UnknownSizes=0;"
		"Socket=8192;"
		"Fetch=100;"
		"ConnSettings=;"
		"ShowSystemTables=0;"
		"RowVersioning=0;"
		"ShowOidColumn=0;"
		"FakeOidIndex=0;"
		"ReadOnly=0;", driver);

	return true;
}


void ODBCPostgreSQLTest::setUp()
{
}


void ODBCPostgreSQLTest::tearDown()
{
	dropObject("TABLE", "Person");
	dropObject("TABLE", "Strings");
	dropObject("TABLE", "Tuples");
}


bool ODBCPostgreSQLTest::init(const std::string& driver, const std::string& dsn)
{
	Utility::drivers(_drivers);
	if (!canConnect(driver, dsn)) return false;
	
	ODBC::Connector::registerConnector();
	try
	{
		_pSession = new Session(ODBC::Connector::KEY, _dbConnString);
	}catch (ConnectionException& ex)
	{
		std::cout << ex.toString() << std::endl;
		return false;
	}

	if (_pSession && _pSession->isConnected()) 
		std::cout << "*** Connected to [" << driver << "] test database." << std::endl;

	_pExecutor = new SQLExecutor(driver + " SQL Executor", _pSession);

	return true;
}


CppUnit::Test* ODBCPostgreSQLTest::suite()
{
#ifndef POCO_ODBC_USE_MAMMOTH_NG
	if (init("PostgreSQL ANSI", "PocoDataPostgreSQLTest"))
#else
	if (init("Mammoth ODBCng Beta", "Mammoth ODBCng Beta"))
#endif
	{
		CppUnit::TestSuite* pSuite = new CppUnit::TestSuite("ODBCPostgreSQLTest");

		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testBareboneODBC);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testSimpleAccess);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testComplexType);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testSimpleAccessVector);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testComplexTypeVector);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testInsertVector);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testInsertEmptyVector);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testSimpleAccessList);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testComplexTypeList);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testInsertList);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testInsertEmptyList);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testSimpleAccessDeque);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testComplexTypeDeque);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testInsertDeque);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testInsertEmptyDeque);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testInsertSingleBulk);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testInsertSingleBulkVec);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testLimit);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testLimitOnce);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testLimitPrepare);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testLimitZero);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testPrepare);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testSetSimple);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testSetComplex);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testSetComplexUnique);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testMultiSetSimple);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testMultiSetComplex);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testMapComplex);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testMapComplexUnique);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testMultiMapComplex);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testSelectIntoSingle);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testSelectIntoSingleStep);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testSelectIntoSingleFail);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testLowerLimitOk);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testLowerLimitFail);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testCombinedLimits);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testCombinedIllegalLimits);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testRange);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testIllegalRange);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testSingleSelect);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testEmptyDB);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testBLOB);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testBLOBStmt);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testDateTime);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testFloat);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testDouble);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testTuple);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testTupleVector);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testInternalExtraction);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testInternalStorageType);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testStoredFunction);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testNull);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testRowIterator);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testStdVectorBool);
		CppUnit_addTest(pSuite, ODBCPostgreSQLTest, testAsync);

		return pSuite;
	}

	return 0;
}
