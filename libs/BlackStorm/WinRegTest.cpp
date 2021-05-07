//////////////////////////////////////////////////////////////////////////
//
// WinRegTest.cpp -- by Giovanni Dicanio
// 
// Test some of the code in WinReg.hpp
// 
// NOTE --- Test Preparation ---
// In the folder containing this source file, there should be also a file 
// "GioTest.reg". This REG file contains some initial data to load into 
// the registry for this test.
// 
////////////////////////////////////////////////////////////////////////// 

#include "WinReg.hpp"   // Module to test
#include <exception>
#include <iostream>
#include <vector>
#include "PPBase.h"

using namespace winreg;

int win_reg_test_main()
{
     int kExitOk = 0;
     int kExitError = 1;

    try 
    {
		ccout << TEXT("=========================================\n");
		ccout << TEXT("*** Testing Giovanni Dicanio's WinReg ***\n");
		ccout << TEXT("=========================================\n\n");

        //
        // Test subkey and value enumeration
        // 

        const cstring testSubKey = TEXT("SOFTWARE\\GioTest");
        RegKey key( HKEY_CURRENT_USER, testSubKey );    
        
		std::vector<cstring> subKeyNames = key.EnumSubKeys();
		ccout << TEXT("Subkeys:\n");
        for ( auto i=0;i<subKeyNames.size();i++ )
        {
			ccout << TEXT("[") << subKeyNames[i] << TEXT("]\n");
        }
		ccout << TEXT('\n');

		std::vector<std::pair<cstring, DWORD>> values = key.EnumValues();
		ccout << TEXT("Values:\n");
        for ( auto i=0;i<values.size();i++)
        {
			ccout << TEXT("  [") << values[i].first << TEXT("](") << RegKey::RegTypeToString(values[i].second) << L")\n";
        }
		ccout << L'\n';

        key.Close();


        //
        // Test SetXxxValue and GetXxxValue methods
        // 
        
        key.Open(HKEY_CURRENT_USER, testSubKey);
        
        const DWORD testDw = 0x1234ABCD;
        const ULONGLONG testQw = 0xAABBCCDD11223344;
        const cstring testSz = L"CiaoTestSz";
        const cstring testExpandSz = L"%PATH%";
        std::vector<BYTE> testBinary;
		testBinary.push_back(0x11);

        std::vector<cstring> testMultiSz ;
		testMultiSz.push_back(L"HI");

        key.SetDwordValue(L"TestValueDword", testDw);
        key.SetQwordValue(L"TestValueQword", testQw);
        key.SetStringValue(L"TestValueString", testSz);
        key.SetExpandStringValue(L"TestValueExpandString", testExpandSz);
        key.SetMultiStringValue(L"TestValueMultiString", testMultiSz);
        key.SetBinaryValue(L"TestValueBinary", testBinary);

        DWORD testDw1 = key.GetDwordValue(L"TestValueDword");
        if (testDw1 != testDw) 
        {
            ccout << L"RegKey::GetDwordValue failed.\n";
        }

        DWORD typeId = key.QueryValueType(L"TestValueDword");
        if (typeId != REG_DWORD)
        {
            ccout << L"RegKey::QueryValueType failed for REG_DWORD.\n";
        }

        ULONGLONG testQw1 = key.GetQwordValue(L"TestValueQword");
        if (testQw1 != testQw)
        {
            ccout << L"RegKey::GetQwordValue failed.\n";
        }

        typeId = key.QueryValueType(L"TestValueQword");
        if (typeId != REG_QWORD)
        {
            ccout << L"RegKey::QueryValueType failed for REG_QWORD.\n";
        }

        cstring testSz1 = key.GetStringValue(L"TestValueString");
        if (testSz1 != testSz) 
        {
            ccout << L"RegKey::GetStringValue failed.\n";
        }

        typeId = key.QueryValueType(L"TestValueString");
        if (typeId != REG_SZ)
        {
            ccout << L"RegKey::QueryValueType failed for REG_SZ.\n";
        }

        cstring testExpandSz1 = key.GetExpandStringValue(L"TestValueExpandString");
        if (testExpandSz1 != testExpandSz)
        {
            ccout << L"RegKey::GetExpandStringValue failed.\n";
        }

        typeId = key.QueryValueType(L"TestValueExpandString");
        if (typeId != REG_EXPAND_SZ)
        {
            ccout << L"RegKey::QueryValueType failed for REG_EXPAND_SZ.\n";
        }

		std::vector<cstring> testMultiSz1 = key.GetMultiStringValue(L"TestValueMultiString");
        if (testMultiSz1 != testMultiSz)
        {
            ccout << L"RegKey::GetMultiStringValue failed.\n";
        }

        typeId = key.QueryValueType(L"TestValueMultiString");
        if (typeId != REG_MULTI_SZ)
        {
            ccout << L"RegKey::QueryValueType failed for REG_MULTI_SZ.\n";
        }

		std::vector<BYTE> testBinary1 = key.GetBinaryValue(L"TestValueBinary");
        if (testBinary1 != testBinary)
        {
            ccout << L"RegKey::GetBinaryValue failed.\n";
        }

        typeId = key.QueryValueType(L"TestValueBinary");
        if (typeId != REG_BINARY)
        {
            ccout << L"RegKey::QueryValueType failed for REG_BINARY.\n";
        }


        //
        // Remove some test values
        //
        
        key.DeleteValue(L"TestValueDword");
        key.DeleteValue(L"TestValueQword");
        key.DeleteValue(L"TestValueString");
        key.DeleteValue(L"TestValueExpandString");
        key.DeleteValue(L"TestValueMultiString");
        key.DeleteValue(L"TestValueBinary");

        ccout << L"All right!! :)\n\n";
    }
    catch (const RegException& e)
    {
		ccout << "\n*** Registry Exception: " << e.what();
		ccout << "\n*** [Windows API error code = " << e.ErrorCode() << "\n\n";
        return kExitError;
    }
    catch (const std::exception& e)
    {
		ccout << "\n*** ERROR: " << e.what() << '\n';
        return kExitError;
    }

    return kExitOk;
}
