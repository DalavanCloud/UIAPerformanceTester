/*
Copyright (C) 2017 NV Access Limited.
This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "stdafx.h"
#include "UIAWrappers.h"
#include "UIAUtils.h"
#include "UIASerializer.h"
#include "testUtils.h"


using namespace WEX::Logging;
using namespace WEX::Common;
using namespace WEX::TestExecution;

// TAEF test module

// Max time in seconds for fetching a line
const float maxLineTimeout=0.2;

// The UIAutomation client instance all tests use
CComPtr<IUIAutomation> UIAClient;

// TAEF module set-up
MODULE_SETUP(moduleInit) {
	// Beep to indecate start of test run (As other AT should be turned off while running)
	Beep(220, 100);
	// Initialize COM and create a UIAutomation client for the tests.
	CoInitialize(NULL);
	UIAClient = createUIAClient();
	return true;
}

// A TAEF test class for testing UI Automation in various applications 
class Tests {
	TEST_CLASS(Tests);
	// a public URL to the test files on the web.
	// Edge in WDAG cannot read local files, thus they must be from the web.
	const std::wstring testFilesURL{L"https://rawgit.com/nvaccess/UIAPerformanceTester/master/UIAPerformanceTester/testFiles/"};

	// Setup method.
	// Just beeps to denote a test is starting
	TEST_METHOD_SETUP(methodInit) {
		Beep(330,100);
		return true;
	}

	// Times fetching a line in MS word
TEST_METHOD(MSWord_readLine)
	{
		BEGIN_TEST_METHOD_PROPERTIES()
			TEST_METHOD_PROPERTY(L"Description", L"Times fetching of a complex line in MS Word")
			END_TEST_METHOD_PROPERTIES()
			// The URL of the document being tested
			auto docURL = testFilesURL+L"MSWord_tests.docx";
		// Launch Microsoft Word and load the document from the URL 
		// Microsoft Word will close when this object goes out of scope.
		auto wordApp = UWPApp_Word(docURL);
		// Locate the document's UIAutomation element in Word.
		auto document = wordApp.locateDocumentUIAElement(UIAClient);
		// Get the document's textPattern, and the textRange spanning the entire document
		CComQIPtr<IUIAutomationTextPattern> textPattern = UIAElement_GetCurrentPattern(document, UIA_TextPatternId);
		if (!textPattern) VERIFY_FAIL(L"document element has no text pattern");
		// Fetch the range for the document 
		CComPtr<IUIAutomationTextRange> textRange = UIATextPattern_GetDocumentRange(textPattern);
		// collapse the range to the start
		UIATextRange_MoveEndpointByRange(textRange, TextPatternRangeEndpoint_End, textRange, TextPatternRangeEndpoint_Start);
		// Move to the second paragraph
		UIATextRange_Move(textRange, TextUnit_Paragraph, 1);
		// Expand to pargraph.
		// Note that we expand to paragraph here rather than line just in case  we are on a small screen and the line is wrapped.
		// this paragraph  only contains one line on average screens.
		UIATextRange_ExpandToEnclosingUnit(textRange, TextUnit_Paragraph);
		//Prepare a UIASerializer, setting the wanted text attributes and element properties
		auto serializer = UIATextContentSerializer(UIAClient, textPattern);
		serializer.registerTextAttribute(L"fontName", UIA_FontNameAttributeId);
		serializer.registerTextAttribute(L"fontSize", UIA_FontSizeAttributeId);
		serializer.registerElementProperty(L"name", UIA_NamePropertyId);
		serializer.registerElementProperty(L"controlType", UIA_LocalizedControlTypePropertyId);
		std::wstring content;
		// sleep for a while to ensure the document is not using much CPU
		Sleep(1000);
		// Time the serialization of the line 
		verifyTakesLessThan(L"UIATextContentSerializer::serializeTextcontent", maxLineTimeout, [&] {
			content = serializer.serializeTextcontent(textRange);
		});
		// Write the serialized content to file
		std::wofstream outFile("MSWord_readLine.xml");
		outFile.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::generate_header>));
		outFile << content;
		// Double check the content with the expected result
		std::wifstream inFile("testFiles\\MSWord_readLine_expected.xml");
		inFile.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::generate_header>));
		std::wstringstream s;
		// Jump over the BOM mark
		inFile.get();
		s << (inFile.rdbuf());
		VERIFY_ARE_EQUAL(0, content.compare(s.str()), L"Comparing serialized content");
	}

// Times fetching a paragraph in MS word
TEST_METHOD(MSWord_readParagraph)
{
	BEGIN_TEST_METHOD_PROPERTIES()
		TEST_METHOD_PROPERTY(L"Description", L"Times fetching of a complex paragraph in MS Word")
		END_TEST_METHOD_PROPERTIES()
		// The URL of the document being tested
		auto docURL = testFilesURL + L"MSWord_tests.docx";
	// Launch Microsoft Word and load the document from the URL 
	// Microsoft Word will close when this object goes out of scope.
	auto wordApp = UWPApp_Word(docURL);
	// Locate the document's UIAutomation element in Word.
	auto document = wordApp.locateDocumentUIAElement(UIAClient);
	// Get the document's textPattern, and the textRange spanning the entire document
	CComQIPtr<IUIAutomationTextPattern> textPattern = UIAElement_GetCurrentPattern(document, UIA_TextPatternId);
	if (!textPattern) VERIFY_FAIL(L"document element has no text pattern");
	// Fetch the range for the document 
	CComPtr<IUIAutomationTextRange> textRange = UIATextPattern_GetDocumentRange(textPattern);
	// collapse the range to the start
	UIATextRange_MoveEndpointByRange(textRange, TextPatternRangeEndpoint_End, textRange, TextPatternRangeEndpoint_Start);
	// Move to the 4th paragraph
	UIATextRange_Move(textRange, TextUnit_Paragraph, 4);
	// Expand to paragraph
	UIATextRange_ExpandToEnclosingUnit(textRange, TextUnit_Paragraph);
	//Prepare a UIASerializer, setting the wanted text attributes and element properties
	auto serializer = UIATextContentSerializer(UIAClient, textPattern);
	serializer.registerTextAttribute(L"fontName", UIA_FontNameAttributeId);
	serializer.registerTextAttribute(L"fontSize", UIA_FontSizeAttributeId);
	serializer.registerElementProperty(L"name", UIA_NamePropertyId);
	serializer.registerElementProperty(L"controlType", UIA_LocalizedControlTypePropertyId);
	std::wstring content;
	// sleep for a while to ensure the document is not using much CPU
	Sleep(1000);
	// Time the serialization of the line 
	verifyTakesLessThan(L"UIATextContentSerializer::serializeTextcontent", maxLineTimeout*5, [&] {
		content = serializer.serializeTextcontent(textRange);
	});
	// Write the serialized content to file
	std::wofstream outFile("MSWord_readParagraph.xml");
	outFile.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::generate_header>));
	outFile << content;
	// Double check the content with the expected result
	std::wifstream inFile("testFiles\\MSWord_readParagraph_expected.xml");
	inFile.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::generate_header>));
	std::wstringstream s;
	// Jump over the BOM mark
	inFile.get();
	s << (inFile.rdbuf());
	VERIFY_ARE_EQUAL(0, content.compare(s.str()), L"Comparing serialized content");
}

	// a helper method to test how long it takes to serialize all content with UIAutomation from a horizontal navbar in Microsoft Edge.
	// this horizontal navbar is a list containing 7 links.
	// This method can either use standard Microsoft Edge, or Edge within Windows Defender Application Guard (WDAG).
	// The method can also optionally dump the serialized content to a file (for regenerating the xpected output).
	void testHorizontalNavbar_helper(bool inWDAG, bool shouldDumpToFile=false) {
			// the title of the document being tested
		const std::wstring docTitle{ L"UIAPerformanceTester:horizontalNavbar" };
		// The URL of the document being tested
		auto docURL = testFilesURL + L"edgeDocument_horizontalNavbar.html";
		// Launch Microsoft Edge (either standard or WDAG) and load the document from the URL.
		// Microsoft Edge will close when this object goes out of scope.
		auto edgeApp = UWPApp_Edge(docURL, inWDAG);
		// Locate the document's UIAutomation element in Edge.
		auto document = edgeApp.locateDocumentUIAElement(UIAClient, docTitle);
		// Get the document's textPattern, and the textRange spanning the entire document
		CComQIPtr<IUIAutomationTextPattern> textPattern = UIAElement_GetCurrentPattern(document, UIA_TextPatternId);
		if (!textPattern) VERIFY_FAIL(L"document element has no text pattern");
		// Fetch the range for the document 
		CComPtr<IUIAutomationTextRange> textRange = UIATextPattern_GetDocumentRange(textPattern);
		// collapse the range to the start
		UIATextRange_MoveEndpointByRange(textRange, TextPatternRangeEndpoint_End, textRange, TextPatternRangeEndpoint_Start);
		// Move to the second line
		UIATextRange_Move(textRange, TextUnit_Line, 1);
		// Expand to line (covering the horizontal navbar)
		UIATextRange_ExpandToEnclosingUnit(textRange, TextUnit_Line);
		//Prepare a UIASerializer, setting the wanted text attributes and element properties
		auto serializer = UIATextContentSerializer(UIAClient, textPattern);
		serializer.registerTextAttribute(L"fontName", UIA_FontNameAttributeId);
		serializer.registerTextAttribute(L"fontSize", UIA_FontSizeAttributeId);
		serializer.registerElementProperty(L"name", UIA_NamePropertyId);
		serializer.registerElementProperty(L"controlType", UIA_LocalizedControlTypePropertyId);
		std::wstring content;
		// sleep for a while to ensure the document is not using much CPU
		Sleep(1000); 
		// Time the serialization of the horizontal navbar 
			verifyTakesLessThan(L"UIATextContentSerializer::serializeTextcontent", maxLineTimeout, [&] {
			content = serializer.serializeTextcontent(textRange);
		});
		// Write the serialized content to file
		if(shouldDumpToFile) {
			std::wofstream outFile("edgeDocument_horizontalNavbar_output.xml");
			outFile << content;
		}
		// Double check the content with the expected result
		std::wifstream inFile("testFiles\\edgeDocument_horizontalNavbar_expected.xml");
		std::wstringstream s;
		s << inFile.rdbuf();
		VERIFY_ARE_EQUAL(0, content.compare(s.str()), L"Comparing serialized content");
	}

	// A test to time the serialization of a horizontal navbar of links in standard Microsoft Edge
	TEST_METHOD(horizontalNavbar)
	{
		BEGIN_TEST_METHOD_PROPERTIES()
			TEST_METHOD_PROPERTY(L"Description", L"Times fetching a line of links in standard Edge")
			//TEST_METHOD_PROPERTY(L"Ignore", L"true")
		END_TEST_METHOD_PROPERTIES()
		testHorizontalNavbar_helper(false);
	}

	// A test to time the serialization of a horizontal navbar of links in WDAG Edge
	TEST_METHOD(WDAG_horizontalNavbar)
	{
		BEGIN_TEST_METHOD_PROPERTIES()
			TEST_METHOD_PROPERTY(L"Description", L"Times fetching a line of links in Edge (WDAG)")
			//TEST_METHOD_PROPERTY(L"Ignore", L"true")
		END_TEST_METHOD_PROPERTIES()
		testHorizontalNavbar_helper(true);
	}

	// A cleanup method.
	// Just beeps to denote when a test ends.
	TEST_METHOD_CLEANUP(methodCleanup) {
		Beep(660, 100);
		return true;
	}

};

MODULE_CLEANUP(moduleCleanup) {
	UIAClient=nullptr;
	CoUninitialize();
	// Beep to indicate the end of the tests. 
	Beep(880, 100);
	return true;
}
