/*
 * The Apache Software License, Version 1.1
 *
 *
 * Copyright (c) 2001 The Apache Software Foundation.  All rights 
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *	  notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *	  notice, this list of conditions and the following disclaimer in
 *	  the documentation and/or other materials provided with the
 *	  distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *	  if any, must include the following acknowledgment:  
 *		 "This product includes software developed by the
 *		  Apache Software Foundation (http://www.apache.org/)."
 *	  Alternately, this acknowledgment may appear in the software itself,
 *	  if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "Xalan" and "Apache Software Foundation" must
 *	  not be used to endorse or promote products derived from this
 *	  software without prior written permission. For written 
 *	  permission, please contact apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache",
 *	  nor may "Apache" appear in their name, without prior written
 *	  permission of the Apache Software Foundation.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.	IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Software Foundation and was
 * originally based on software copyright (c) 1999, International
 * Business Machines, Inc., http://www.ibm.com.  For more
 * information on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 */
#include "XalanTransformer.hpp"



#include <algorithm>



#include <sax/SAXException.hpp>



#include <XalanDOM/XalanDOMException.hpp>



#include <PlatformSupport/DOMStringHelper.hpp>
#include <PlatformSupport/DOMStringPrintWriter.hpp>
#include <PlatformSupport/XalanOutputStreamPrintWriter.hpp>



#include <XPath/XObjectFactoryDefault.hpp>
#include <XPath/XPathFactoryBlock.hpp>
#include <XPath/XPathFactoryDefault.hpp>



#include <XSLT/StylesheetConstructionContextDefault.hpp>
#include <XSLT/StylesheetExecutionContextDefault.hpp>
#include <XSLT/StylesheetRoot.hpp>
#include <XSLT/XSLTEngineImpl.hpp>
#include <XSLT/XSLTInit.hpp>
#include <XSLT/XSLTProcessorEnvSupportDefault.hpp>



#include <XalanSourceTree/XalanSourceTreeDOMSupport.hpp>
#include <XalanSourceTree/XalanSourceTreeParserLiaison.hpp>



#include "XalanCompiledStylesheetDefault.hpp"
#include "XalanDefaultDocumentBuilder.hpp"
#include "XalanDefaultParsedSource.hpp"
#include "XalanTransformerOutputStream.hpp"
#include "XercesDOMParsedSource.hpp"



XSLTInit*	XalanTransformer::s_xsltInit = 0;



XalanTransformer::XalanTransformer():
	m_compiledStylesheets(),
	m_parsedSources(),
	m_paramPairs(),
	m_functionPairs(),
	m_errorMessage(1, '\0'),
	m_stylesheetExecutionContext(new StylesheetExecutionContextDefault)
{
}



XalanTransformer::~XalanTransformer()
{
#if !defined(XALAN_NO_NAMESPACES)
	using std::for_each;
#endif

	// Clean up all entries in the compliledStylesheets vector.
	for_each(m_compiledStylesheets.begin(),
			 m_compiledStylesheets.end(),
			 DeleteFunctor<XalanCompiledStylesheet>());

	// Clean up all entries in the compliledStylesheets vector.
	for_each(m_parsedSources.begin(),
			 m_parsedSources.end(),
			 DeleteFunctor<XalanParsedSource>());

	for (FunctionParamPairVectorType::size_type i = 0; i < m_functionPairs.size(); ++i)
	{			
		delete m_functionPairs[i].second;
	}

	m_functionPairs.clear();

	delete m_stylesheetExecutionContext;
}



void
XalanTransformer::initialize()
{
	// Initialize Xalan. 
	s_xsltInit = new XSLTInit;
}



void
XalanTransformer::terminate()
{
	// Terminate Xalan and release memory.
	delete s_xsltInit;

	s_xsltInit = 0;
}



int
XalanTransformer::transform(
	const XalanParsedSource&	theParsedXML, 
	const XSLTInputSource&		theStylesheetSource,
	const XSLTResultTarget& 	theResultTarget)
{
#if !defined(XALAN_NO_NAMESPACES)
	using std::for_each;
#endif

	int 	theResult = 0;

	// Clear the error message.
	m_errorMessage.resize(1, '\0');

	// Store error messages from problem listener.
	XalanDOMString	theErrorMessage;

	try
	{
		// Create the helper object that is necessary for running the processor...
		XalanAutoPtr<XalanParsedSourceHelper>	theHelper(theParsedXML.createHelper());
		assert(theHelper.get() != 0);

		DOMSupport& 					theDOMSupport = theHelper->getDOMSupport();

		XMLParserLiaison&				theParserLiaison = theHelper->getParserLiaison();

		// Create some more support objects...
		XSLTProcessorEnvSupportDefault	theXSLTProcessorEnvSupport;

		XObjectFactoryDefault			theXObjectFactory;

		XPathFactoryDefault 			theXPathFactory;

		// Create a processor...
		XSLTEngineImpl	theProcessor(
				theParserLiaison,
				theXSLTProcessorEnvSupport,
				theDOMSupport,
				theXObjectFactory,
				theXPathFactory);

		theXSLTProcessorEnvSupport.setProcessor(&theProcessor);

		// Create a problem listener and send output to a XalanDOMString.
		DOMStringPrintWriter	thePrintWriter(theErrorMessage);
		
		ProblemListenerDefault	theProblemListener(&thePrintWriter);

		theProcessor.setProblemListener(&theProblemListener);

		theParserLiaison.setExecutionContext(*m_stylesheetExecutionContext);

		// Create a stylesheet construction context, 
		// using the stylesheet's factory support objects.
		StylesheetConstructionContextDefault	theStylesheetConstructionContext(
					theProcessor,
					theXSLTProcessorEnvSupport,
					theXPathFactory);

		// Hack used to cast away const.
		XSLTResultTarget	tempResultTarget(theResultTarget);

		const EnsureReset	theReset(*this);

		// Set up the stylesheet execution context.
		m_stylesheetExecutionContext->setXPathEnvSupport(&theXSLTProcessorEnvSupport);

		m_stylesheetExecutionContext->setDOMSupport(&theDOMSupport);

		m_stylesheetExecutionContext->setXObjectFactory(&theXObjectFactory);

		m_stylesheetExecutionContext->setXSLTProcessor(&theProcessor);

		// Set the parameters if any.
		for (ParamPairVectorType::size_type i = 0; i < m_paramPairs.size(); ++i)
		{
			theProcessor.setStylesheetParam(
					m_paramPairs[i].first,
					m_paramPairs[i].second);
		}

		// Set the functions if any.
		for (FunctionParamPairVectorType::size_type f = 0; f < m_functionPairs.size(); ++f)
		{
			theXSLTProcessorEnvSupport.installExternalFunctionLocal(
					m_functionPairs[f].first.getNamespace(),
					m_functionPairs[f].first.getLocalPart(),
					*(m_functionPairs[f].second));
		}

		// Do the transformation...
		theProcessor.process(
					theParsedXML.getDocument(),
					theStylesheetSource,
					tempResultTarget,
					theStylesheetConstructionContext,
					*m_stylesheetExecutionContext);
	}
	catch (XSLException& e)
	{
		if (length(theErrorMessage) != 0)
		{
			TranscodeToLocalCodePage(theErrorMessage, m_errorMessage, true);
		}
		else
		{
			TranscodeToLocalCodePage(e.getMessage(), m_errorMessage, true);
		}

		theResult = -1; 	
	}
	catch (SAXException& e)
	{
		if (length(theErrorMessage) != 0)
		{
			TranscodeToLocalCodePage(theErrorMessage, m_errorMessage, true);
		}
		else
		{
			TranscodeToLocalCodePage(e.getMessage(), m_errorMessage, true);
		}

		theResult = -2;
	}
	catch (XMLException& e)
	{
		if (length(theErrorMessage) != 0)
		{
			TranscodeToLocalCodePage(theErrorMessage, m_errorMessage, true);
		}
		else
		{
			TranscodeToLocalCodePage(e.getMessage(), m_errorMessage, true);
		}

		theResult = -3;
	}
	catch(const XalanDOMException&	e)
	{
		if (length(theErrorMessage) != 0)
		{
			TranscodeToLocalCodePage(theErrorMessage, m_errorMessage, true);
		}
		else
		{
			XalanDOMString theMessage("XalanDOMException caught.  The code is ");
			
			append(theMessage,	LongToDOMString(long(e.getExceptionCode())));
			append(theMessage,	XalanDOMString("."));						 

			TranscodeToLocalCodePage(theMessage, m_errorMessage, true);
		}

		theResult = -4;
	}

	return theResult;
}



int
XalanTransformer::transform(
			const XalanParsedSource&		theParsedXML, 
			const XalanCompiledStylesheet*	theCompiledStylesheet,
			const XSLTResultTarget& 		theResultTarget)
{
#if !defined(XALAN_NO_NAMESPACES)
	using std::for_each;
#endif

	int 	theResult = 0;

	// Clear the error message.
	m_errorMessage.resize(1, '\0');

	// Store error messages from problem listener.
	XalanDOMString	theErrorMessage;

	try
	{
		// Create the helper object that is necessary for running the processor...
		XalanAutoPtr<XalanParsedSourceHelper>	theHelper(theParsedXML.createHelper());
		assert(theHelper.get() != 0);

		DOMSupport& 					theDOMSupport = theHelper->getDOMSupport();

		XMLParserLiaison&				theParserLiaison = theHelper->getParserLiaison();

		// Create some more support objects...
		XSLTProcessorEnvSupportDefault	theXSLTProcessorEnvSupport;

		XObjectFactoryDefault			theXObjectFactory;

		XPathFactoryDefault 			theXPathFactory;

		// Create a processor...
		XSLTEngineImpl	theProcessor(
				theParserLiaison,
				theXSLTProcessorEnvSupport,
				theDOMSupport,
				theXObjectFactory,
				theXPathFactory);

		theXSLTProcessorEnvSupport.setProcessor(&theProcessor);

		// Create a problem listener and send output to a XalanDOMString.
		DOMStringPrintWriter	thePrintWriter(theErrorMessage);
		
		ProblemListenerDefault	theProblemListener(&thePrintWriter);

		theProcessor.setProblemListener(&theProblemListener);

		// Since the result target is not const in our
		// internal intefaces, we'll pass in a local copy
		// of the one provided...
		XSLTResultTarget	tempResultTarget(theResultTarget);

		const EnsureReset	theReset(*this);

		// Set up the stylesheet execution context.
		m_stylesheetExecutionContext->setXPathEnvSupport(&theXSLTProcessorEnvSupport);

		m_stylesheetExecutionContext->setDOMSupport(&theDOMSupport);

		m_stylesheetExecutionContext->setXObjectFactory(&theXObjectFactory);

		m_stylesheetExecutionContext->setXSLTProcessor(&theProcessor);
		
		// Set the compiled stylesheet.
		m_stylesheetExecutionContext->setStylesheetRoot(theCompiledStylesheet->getStylesheetRoot());

		// Set the parameters if any.
		for (ParamPairVectorType::size_type i = 0; i < m_paramPairs.size(); ++i)
		{
			theProcessor.setStylesheetParam(
					m_paramPairs[i].first,
					m_paramPairs[i].second);
		}

		// Set the functions if any.
		for (FunctionParamPairVectorType::size_type f = 0; f < m_functionPairs.size(); ++f)
		{
			theXSLTProcessorEnvSupport.installExternalFunctionLocal(
					m_functionPairs[f].first.getNamespace(),
					m_functionPairs[f].first.getLocalPart(),
					*(m_functionPairs[f].second));
		}

		// Do the transformation...
		theProcessor.process(
					theParsedXML.getDocument(), 	
					tempResultTarget,					
					*m_stylesheetExecutionContext);
	}
	catch (XSLException& e)
	{
		if (length(theErrorMessage) != 0)
		{
			TranscodeToLocalCodePage(theErrorMessage, m_errorMessage, true);
		}
		else
		{
			TranscodeToLocalCodePage(e.getMessage(), m_errorMessage, true);
		}

		theResult = -1; 	
	}
	catch (SAXException& e)
	{
		if (length(theErrorMessage) != 0)
		{
			TranscodeToLocalCodePage(theErrorMessage, m_errorMessage, true);
		}
		else
		{
			TranscodeToLocalCodePage(e.getMessage(), m_errorMessage, true);
		}

		theResult = -2;
	}
	catch (XMLException& e)
	{
		if (length(theErrorMessage) != 0)
		{
			TranscodeToLocalCodePage(theErrorMessage, m_errorMessage, true);
		}
		else
		{
			TranscodeToLocalCodePage(e.getMessage(), m_errorMessage, true);
		}

		theResult = -3;
	}
	catch(const XalanDOMException&	e)
	{
		if (length(theErrorMessage) != 0)
		{
			TranscodeToLocalCodePage(theErrorMessage, m_errorMessage, true);
		}
		else
		{
			XalanDOMString theMessage("XalanDOMException caught.  The code is ");
			
			append(theMessage,	LongToDOMString(long(e.getExceptionCode())));
			append(theMessage,	XalanDOMString("."));						 

			TranscodeToLocalCodePage(theMessage, m_errorMessage, true);
		}

		theResult = -4;
	}

	return theResult;
}



int
XalanTransformer::transform(
			const XSLTInputSource&			theInputSource, 
			const XalanCompiledStylesheet*	theCompiledStylesheet,
			const XSLTResultTarget& 		theResultTarget)
{
	const XalanParsedSource*	theParsedSource = 0;
 
	const int	theResult = parseSource(theInputSource, theParsedSource);
 
	if (theResult != 0)
	{
		return theResult;
	}
	else
	{
		assert(theParsedSource != 0);
 
		// Make sure the parsed source is destroyed when
		// the transformation is finished...
		EnsureDestroyParsedSource	theGuard(*this, theParsedSource);

		// Do the transformation...
		return transform(
						*theParsedSource,
						theCompiledStylesheet,
						theResultTarget);
	}
}

int
XalanTransformer::transform(
	const XSLTInputSource&		theInputSource, 
	const XSLTInputSource&		theStylesheetSource,
	const XSLTResultTarget& 	theResultTarget)
{
	// Parse the source document.
	const XalanParsedSource*	theParsedSource = 0;

	const int	theResult = parseSource(theInputSource, theParsedSource);

	if (theResult != 0)
	{
		return theResult;
	}
	else
	{
		assert(theParsedSource != 0);

		// Make sure the parsed source is destroyed when
		// the transformation is finished...
		EnsureDestroyParsedSource	theGuard(*this, theParsedSource);

		// Do the transformation...
		return transform(
						*theParsedSource,
						theStylesheetSource,
						theResultTarget);
	}
}



int
XalanTransformer::transform(
			const XSLTInputSource&		theInputSource, 		
			const XSLTResultTarget& 	theResultTarget)
{
	// Do the transformation...
	return transform(
					theInputSource, 
					XSLTInputSource(),
					theResultTarget);
}



int
XalanTransformer::transform(
			const XSLTInputSource&	theInputSource, 
			const XSLTInputSource&	theStylesheetSource,
			void*					theOutputHandle, 
			XalanOutputHandlerType	theOutputHandler,
			XalanFlushHandlerType	theFlushHandler)
{
	// Set to output target to the callback 
	XalanTransformerOutputStream	theOutputStream(theOutputHandle, theOutputHandler, theFlushHandler);

	XalanOutputStreamPrintWriter	thePrintWriter(theOutputStream);

	XSLTResultTarget				theResultTarget(&thePrintWriter);

	// Do the transformation...
	return transform(
					theInputSource, 
					theStylesheetSource,
					theResultTarget);
}



int
XalanTransformer::transform(
			const XalanParsedSource&		theParsedSource, 
			const XalanCompiledStylesheet*	theCompiledStylesheet,
			void*							theOutputHandle, 
			XalanOutputHandlerType			theOutputHandler,
			XalanFlushHandlerType			theFlushHandler)
{
	// Set to output target to the callback 
	XalanTransformerOutputStream	theOutputStream(theOutputHandle, theOutputHandler, theFlushHandler);

	XalanOutputStreamPrintWriter	thePrintWriter(theOutputStream);

	XSLTResultTarget				theResultTarget(&thePrintWriter);

	// Do the transformation...
	return transform(
					theParsedSource,
					theCompiledStylesheet,
					theResultTarget);
}



int
XalanTransformer::transform(
			const XSLTInputSource&		theInputSource, 		
			void*						theOutputHandle, 
			XalanOutputHandlerType		theOutputHandler,
			XalanFlushHandlerType		theFlushHandler)
{
	// Set to output target to the callback 
	XalanTransformerOutputStream	theOutputStream(theOutputHandle, theOutputHandler, theFlushHandler);

	XalanOutputStreamPrintWriter	thePrintWriter(theOutputStream);

	XSLTResultTarget				theResultTarget(&thePrintWriter);

	// Do the transformation...
	return transform(
					theInputSource, 
					XSLTInputSource(),
					theResultTarget);
}



int
XalanTransformer::compileStylesheet(
			const XSLTInputSource&				theStylesheetSource,
			const XalanCompiledStylesheet*& 	theCompiledStylesheet)
{
	// Clear the error message.
	m_errorMessage.resize(1, '\0');

	// Store error messages from problem listener.
	XalanDOMString	theErrorMessage;

	int 			theResult = 0;

	try
	{
		// Create some support objects that are necessary for running the processor...
		XalanSourceTreeDOMSupport		theDOMSupport;

		XalanSourceTreeParserLiaison	theParserLiaison(theDOMSupport);

		// Hook the two together...
		theDOMSupport.setParserLiaison(&theParserLiaison);

		// Create some more support objects...
		XSLTProcessorEnvSupportDefault	theXSLTProcessorEnvSupport;

		XObjectFactoryDefault			theXObjectFactory;

		XPathFactoryDefault 			theXPathFactory;

		// Create a processor...
		XSLTEngineImpl	theProcessor(
				theParserLiaison,
				theXSLTProcessorEnvSupport,
				theDOMSupport,
				theXObjectFactory,
				theXPathFactory);

		// Create a problem listener and send output to a XalanDOMString.
		DOMStringPrintWriter	thePrintWriter(theErrorMessage);

		ProblemListenerDefault	theProblemListener(&thePrintWriter);

		theProcessor.setProblemListener(&theProblemListener);

		// Create a new XalanCompiledStylesheet.
		theCompiledStylesheet =
			new XalanCompiledStylesheetDefault(
						theStylesheetSource,
						theXSLTProcessorEnvSupport,
						theProcessor);

		// Store it in a vector.
		m_compiledStylesheets.push_back(theCompiledStylesheet);
	}
	catch (XSLException& e)
	{
		if (length(theErrorMessage) != 0)
		{
			TranscodeToLocalCodePage(theErrorMessage, m_errorMessage, true);
		}
		else
		{
			TranscodeToLocalCodePage(e.getMessage(), m_errorMessage, true);
		}

		theResult = -1;
	}
	catch (SAXException& e)
	{
		if (length(theErrorMessage) != 0)
		{
			TranscodeToLocalCodePage(theErrorMessage, m_errorMessage, true);
		}
		else
		{
			TranscodeToLocalCodePage(e.getMessage(), m_errorMessage, true);
		}

		theResult = -2;
	}
	catch (XMLException& e)
	{
		if (length(theErrorMessage) != 0)
		{
			TranscodeToLocalCodePage(theErrorMessage, m_errorMessage, true);
		}
		else
		{
			TranscodeToLocalCodePage(e.getMessage(), m_errorMessage, true);
		}

		theResult = -3;
	}
	catch(const XalanDOMException&	e)
	{
		if (length(theErrorMessage) != 0)
		{
			TranscodeToLocalCodePage(theErrorMessage, m_errorMessage, true);
		}
		else
		{
			XalanDOMString theMessage("XalanDOMException caught.  The code is ");

			append(theMessage,	LongToDOMString(long(e.getExceptionCode())));

			append(theMessage,	XalanDOMString("."));

			TranscodeToLocalCodePage(theMessage, m_errorMessage, true);
		}

		theResult = -4;
	}

	return theResult;
}



int
XalanTransformer::destroyStylesheet(const XalanCompiledStylesheet*	theStylesheet)
{
#if !defined(XALAN_NO_NAMESPACES)
	using std::find;
#endif

	const CompiledStylesheetPtrVectorType::iterator 	i =
		find(
			m_compiledStylesheets.begin(),
			m_compiledStylesheets.end(),
			theStylesheet);

	if (i == m_compiledStylesheets.end())
	{
		const char* const	theStylesheetErrorMessage =
				"An invalid compiled stylesheet was provided.";

		const unsigned int	theLength =
			length(theStylesheetErrorMessage);

		m_errorMessage.resize(theLength + 1, '\0');

		strncpy(&*m_errorMessage.begin(), theStylesheetErrorMessage, theLength);

		return -1;
	}
	else
	{
		m_compiledStylesheets.erase(i);

#if defined(XALAN_CANNOT_DELETE_CONST)
		delete (XalanCompiledStylesheet*) theStylesheet;
#else
		delete theStylesheet;
#endif

		return 0;
	}
}



int
XalanTransformer::parseSource(
			const XSLTInputSource&		theInputSource,
			const XalanParsedSource*&	theParsedSource,
			bool						useXercesDOM)
{
	// Clear the error message.
	m_errorMessage.clear();
	m_errorMessage.push_back(0);

	int theResult = 0;

	try
	{
		if(useXercesDOM == true)
		{
			theParsedSource = new XercesDOMParsedSource(theInputSource);
		}
		else
		{
			theParsedSource = new XalanDefaultParsedSource(theInputSource);
		}

		// Store it in a vector.
		m_parsedSources.push_back(theParsedSource);
	}
	catch (XSLException& e)
	{
		TranscodeToLocalCodePage(e.getMessage(), m_errorMessage, true);

		theResult = -1;
	}
	catch (SAXException& e)
	{
		TranscodeToLocalCodePage(e.getMessage(), m_errorMessage, true);

		theResult = -2;
	}
	catch (XMLException& e)
	{
		TranscodeToLocalCodePage(e.getMessage(), m_errorMessage, true);

		theResult = -3;
	}
	catch(const XalanDOMException&	e)
	{
		XalanDOMString theMessage("XalanDOMException caught.  The code is ");
			
		append(theMessage,	LongToDOMString(long(e.getExceptionCode())));

		append(theMessage,	XalanDOMString("."));						 

		TranscodeToLocalCodePage(theMessage, m_errorMessage, true);

		theResult = -3;
	}

	return theResult;
}



int
XalanTransformer::destroyParsedSource(const XalanParsedSource*	theParsedSource)
{
#if !defined(XALAN_NO_NAMESPACES)
	using std::find;
#endif

	const ParsedSourcePtrVectorType::iterator	i =
		find(
			m_parsedSources.begin(),
			m_parsedSources.end(),
			theParsedSource);

	if (i == m_parsedSources.end())
	{
		const char* const	theParsedSourceErrorMessage =
				"An invalid parsed source was provided.";

		const unsigned int	theLength =
			length(theParsedSourceErrorMessage);

		m_errorMessage.resize(theLength + 1, '\0');

		strncpy(&*m_errorMessage.begin(), theParsedSourceErrorMessage, theLength);

		return -1;
	}
	else
	{
		m_parsedSources.erase(i);

#if defined(XALAN_CANNOT_DELETE_CONST)
		delete (XalanCompiledStylesheet*) theParsedSource;
#else
		delete theParsedSource;
#endif

		return 0;
	}
}



void
XalanTransformer::setStylesheetParam(
			const XalanDOMString&	key,
			const XalanDOMString&	expression)
{
	// Store the stylesheet parameter in a vector.
	m_paramPairs.push_back(ParamPairType(key,  expression));
}



XalanDocumentBuilder*
XalanTransformer::createDocumentBuilder()
{
	m_parsedSources.reserve(m_parsedSources.size() + 1);

	XalanDocumentBuilder* const 	theNewBuilder = new XalanDefaultDocumentBuilder;

	m_parsedSources.push_back(theNewBuilder);

	return theNewBuilder;
}



void
XalanTransformer::destroyDocumentBuilder(XalanDocumentBuilder*	theDocumentBuilder)
{
	destroyParsedSource(theDocumentBuilder);
}



void
XalanTransformer::installExternalFunction(
			const XalanDOMString&	theNamespace,
			const XalanDOMString&	functionName,
			const Function& 		function)
{
	m_functionPairs.push_back(FunctionPairType(XalanQNameByValue(theNamespace, functionName), function.clone()));
}



void
XalanTransformer::installExternalFunctionGlobal(
			const XalanDOMString&	theNamespace,
			const XalanDOMString&	functionName,
			const Function& 		function)
{
	XSLTProcessorEnvSupportDefault::installExternalFunctionGlobal(
			theNamespace,
			functionName,
			function);
}



void
XalanTransformer::uninstallExternalFunction(
			const XalanDOMString&	theNamespace,
			const XalanDOMString&	functionName)
{
	for (FunctionParamPairVectorType::size_type i = 0; i < m_functionPairs.size(); ++i)
	{
		if(XalanQNameByReference(theNamespace, functionName).equals(m_functionPairs[i].first))
		{
			delete m_functionPairs[i].second;

			m_functionPairs.erase(m_functionPairs.begin() + i); 	
		}
	}	
}



void
XalanTransformer::uninstallExternalFunctionGlobal(
			const XalanDOMString&	theNamespace,
			const XalanDOMString&	functionName)
{
	XSLTProcessorEnvSupportDefault::uninstallExternalFunctionGlobal(
			theNamespace,
			functionName);
}




const char*
XalanTransformer::getLastError() const
{
	return &m_errorMessage[0]; 
}



void
XalanTransformer::reset()
{
	try
	{
		// Reset objects.
		m_stylesheetExecutionContext->setXPathEnvSupport(0);

		m_stylesheetExecutionContext->setDOMSupport(0);
		
		m_stylesheetExecutionContext->setXObjectFactory(0);
		
		m_stylesheetExecutionContext->setXSLTProcessor(0);

		m_stylesheetExecutionContext->reset();

		// Clear the ParamPairVectorType.
		m_paramPairs.clear();
	}
	catch(...)
	{
	}
}




XalanTransformer::EnsureReset::~EnsureReset()
{
	m_transformer.m_stylesheetExecutionContext->reset();

	m_transformer.reset();
}
