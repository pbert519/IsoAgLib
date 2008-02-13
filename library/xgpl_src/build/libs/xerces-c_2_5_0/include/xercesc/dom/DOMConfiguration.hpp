/*
 * The Apache Software License, Version 1.1
 *
 * Copyright (c) 2003 The Apache Software Foundation.  All rights
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:
 *       "This product includes software developed by the
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "Xerces" and "Apache Software Foundation" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact apache\@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache",
 *    nor may "Apache" appear in their name, without prior written
 *    permission of the Apache Software Foundation.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
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
 * individuals on behalf of the Apache Software Foundation, and was
 * originally based on software copyright (c) 2001, International
 * Business Machines, Inc., http://www.ibm.com .  For more information
 * on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 */

#if !defined(DOMCONFIGURATION_HPP)
#define DOMCONFIGURATION_HPP

//------------------------------------------------------------------------------------
//  Includes
//------------------------------------------------------------------------------------

#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_BEGIN

/**
 *   The DOMConfiguration interface represents the configuration of
 *   a document  and  maintains  a  table of recognized parameters.
 *   using  the   configuration,   it   is   possible   to   change
 *   Document.normalizeDocument  behavior,  such  as replacing
 *   CDATASection   nodes   with   Text  nodes  or
 *   specifying  the  type  of the schema that must be used when the
 *   validation of the Document is requested. DOMConfiguration
 *   objects  are  also used in [DOM Level 3 Load and Save] in
 *   the DOMBuilder and DOMWriter interfaces.
 *
 *   The  DOMConfiguration  distinguish  two  types  of  parameters:
 *   boolean     (boolean    parameters)    and    DOMUserData
 *   (parameters). The names used by the DOMConfiguration object are
 *   defined  throughout  the  DOM Level 3 specifications. Names are
 *   case-insensitives.   To   avoid   possible   conflicts,   as  a
 *   convention,   names   referring   to   boolean  parameters  and
 *   parameters defined outside the DOM specification should be made
 *   unique.  Names  are  recommended  to  follow the XML name
 *   production   rule   but   it   is   not  enforced  by  the  DOM
 *   implementation.  DOM  Level 3 Core Implementations are required
 *   to  recognize  all boolean parameters and parameters defined in
 *   this  specification.  Each boolean parameter state or parameter
 *   value may then be supported or not by the implementation. Refer
 *   to  their  definition  to  know  if  a state or a value must be
 *   supported or not.
 *
 *   Note: Parameters are similar to features and properties used in
 *   SAX2 [SAX].
 *
 * Issue DOMConfiguration-1:
 *         Can we rename boolean parameters to "flags"?
 *
 * Issue DOMConfiguration-2:
 *         Are  boolean  parameters  and  parameters within the same
 *         scope for uniqueness? Which exception should be raised by
 *         setBooleanParameter("error-handler", true)?
 *
 *   The following list of parameters defined in the DOM:
 *
 * "error-handler"
 *         [required]
 *         A   DOMErrorHandler   object.   If   an   error  is
 *         encountered in the document, the implementation will call
 *         back  the  DOMErrorHandler  registered  using  this
 *         parameter.
 *         When  called, DOMError.relatedData will contain the
 *         closest   node   to  where  the  error  occured.  If  the
 *         implementation  is unable to determine the node where the
 *         error occurs, DOMError.relatedData will contain the
 *         Document  node.  Mutations  to  the  document  from
 *         within  an  error  handler  will result in implementation
 *         dependent behaviour.
 *
 *       Issue DOMConfiguration-4:
 *               Should   we   say  non  "readonly"  operations  are
 *               implementation dependent instead?
 *               Resolution:  Removed:  "or re-invoking a validation
 *               operation".
 *
 * "schema-type"
 *         [optional]
 *         A  DOMString  object containing an absolute URI and
 *         representing  the  type  of  the  schema language used to
 *         validate   a  document  against.  Note  that  no  lexical
 *         checking is done on the absolute URI.
 *         If  this  parameter  is  not  set, a default value may be
 *         provided  by  the  implementation,  based  on  the schema
 *         languages  supported  and  on the schema language used at
 *         load time.
 *
 *         Note:   For   XML   Schema  [XML  Schema  Part  1],
 *         applications must use the value
 *         "http://www.w3.org/2001/XMLSchema".     For    XML    DTD
 *         [XML   1.0],   applications   must  use  the  value
 *         "http://www.w3.org/TR/REC-xml".  Other  schema  languages
 *         are  outside  the  scope  of the W3C and therefore should
 *         recommend an absolute URI in order to use this method.
 *
 * "schema-location"
 *         [optional]
 *         A  DOMString  object  containing  a  list  of URIs,
 *         separated   by  white  spaces  (characters  matching  the
 *         nonterminal  production  S  defined  in section 2.3
 *         [XML  1.0]),  that  represents  the schemas against
 *         which  validation  should  occur.  The  types  of schemas
 *         referenced  in  this  list  must match the type specified
 *         with   schema-type,   otherwise   the   behaviour  of  an
 *         implementation  is  undefined.  If the schema type is XML
 *         Schema  [XML  Schema  Part  1], only one of the XML
 *         Schemas in the list can be with no namespace.
 *         If  validation  occurs  against a namespace aware schema,
 *         i.e.  XML  Schema,  and  the  targetNamespace of a schema
 *         (specified    using    this    property)    matches   the
 *         targetNamespace  of  a  schema  occurring in the instance
 *         document,  i.e  in  schemaLocation  attribute, the schema
 *         specified  by  the  user using this property will be used
 *         (i.e.,  in XML Schema the schemaLocation attribute in the
 *         instance  document  or  on  the  import  element  will be
 *         effectively ignored).
 *
 *         Note:  It is illegal to set the schema-location parameter
 *         if  the  schema-type  parameter  value  is not set. It is
 *         strongly  recommended that DOMInputSource.baseURI will be
 *         set,  so  that an implementation can successfully resolve
 *         any external entities referenced.
 *
 *   The  following list of boolean parameters (features) defined in
 *   the DOM:
 *
 * "canonical-form"
 *
 *       true
 *               [optional]
 *               Canonicalize  the  document  according to the rules
 *               specified  in [Canonical XML]. Note that this
 *               is  limited  to what can be represented in the DOM.
 *               In particular, there is no way to specify the order
 *               of the attributes in the DOM.
 *
 *             Issue normalizationFeature-14:
 *                     What  happen  to  other  features?  are  they
 *                     ignored? if yes, how do you know if a feature
 *                     is ignored?
 *
 *       false
 *               [required] (default)
 *               Do not canonicalize the document.
 *
 * "cdata-sections"
 *
 *       true
 *               [required] (default)
 *               Keep CDATASection nodes in the document.
 *
 *             Issue normalizationFeature-11:
 *                     Name  does not work really well in this case.
 *                     ALH     suggests     renaming     this     to
 *                     "cdata-sections".  It works for both load and
 *                     save.
 *                     Resolution:  Renamed as suggested. (Telcon 27
 *                     Jan 2002).
 *
 *       false
 *               [required]
 *               Transform  CDATASection nodes in the document
 *               into  Text  nodes. The new Text node is
 *               then combined with any adjacent Text node.
 *
 * "comments"
 *
 *       true
 *               [required] (default)
 *               Keep Comment nodes in the document.
 *
 *       false
 *               [required]
 *               Discard Comment nodes in the Document.
 *
 * "datatype-normalization"
 *
 *       true
 *               [required]
 *               Exposed normalized values in the tree.
 *
 *             Issue normalizationFeature-8:
 *                     We should define "datatype normalization".
 *                     Resolution:  DTD  normalization  always apply
 *                     because  it's  part  of  XML 1.0. Clarify the
 *                     spec. (Telcon 27 Jan 2002).
 *
 *       false
 *               [required] (default)
 *               Do not perform normalization on the tree.
 *
 * "discard-default-content"
 *
 *       true
 *               [required] (default)
 *               Use   whatever   information   available   to   the
 *               implementation (i.e. XML schema, DTD, the specified
 *               flag on Attr nodes, and so on) to decide what
 *               attributes  and content should be discarded or not.
 *               Note that the specified flag on Attr nodes in
 *               itself  is not always reliable, it is only reliable
 *               when  it  is set to false since the only case where
 *               it  can  be  set  to  false is if the attribute was
 *               created  by the implementation. The default content
 *               won't be removed if an implementation does not have
 *               any information available.
 *
 *             Issue normalizationFeature-2:
 *                     How  does  exactly  work?  What's the comment
 *                     about level 1 implementations?
 *                     Resolution:  Remove  "Level 1" (Telcon 16 Jan
 *                     2002).
 *
 *       false
 *               [required]
 *               Keep all attributes and all content.
 *
 * "entities"
 *
 *       true
 *               [required]
 *               Keep  EntityReference  and Entity nodes
 *               in the document.
 *
 *             Issue normalizationFeature-9:
 *                     How does that interact with
 *                     expand-entity-references?     ALH    suggests
 *                     consolidating  the  two  to  a single feature
 *                     called  "entity-references" that is used both
 *                     for load and save.
 *                     Resolution:  Consolidate both features into a
 *                     single  feature called 'entities'. (Telcon 27
 *                     Jan 2002).
 *
 *       false
 *               [required] (default)
 *               Remove  all  EntityReference and Entity
 *               nodes   from   the  document,  putting  the  entity
 *               expansions  directly  in  their  place.  Text
 *               nodes     are     into    "normal"    form.    Only
 *               EntityReference nodes to non-defined entities
 *               are kept in the document.
 *
 * "infoset"
 *
 *       true
 *               [required]
 *               Only  keep  in the document the information defined
 *               in  the  XML Information Set [XML Information
 *               set].
 *               This   forces  the  following  features  to  false:
 *               namespace-declarations,         validate-if-schema,
 *               entities, datatype-normalization, cdata-sections.
 *               This   forces   the  following  features  to  true:
 *               whitespace-in-element-content,            comments,
 *               namespaces.
 *               Other  features  are  not  changed unless explicity
 *               specified in the description of the features.
 *               Note  that  querying  this  feature with getFeature
 *               returns   true  only  if  the  individual  features
 *               specified above are appropriately set.
 *
 *             Issue normalizationFeature-12:
 *                     Name  doesn't  work  well  here. ALH suggests
 *                     renaming    this   to   limit-to-infoset   or
 *                     match-infoset, something like that.
 *                     Resolution:  Renamed 'infoset' (Telcon 27 Jan
 *                     2002).
 *
 *       false
 *               Setting infoset to false has no effect.
 *
 *             Issue normalizationFeature-13:
 *                     Shouldn't  we  change  this  to  setting  the
 *                     relevant options back to their default value?
 *                     Resolution:   No,   this   is   more  like  a
 *                     convenience  function, it's better to keep it
 *                     simple. (F2F 28 Feb 2002).
 *
 * "namespaces"
 *
 *       true
 *               [required] (default)
 *               Perform  the  namespace  processing  as  defined in
 *               [XML Namespaces].
 *
 *       false
 *               [optional]
 *               Do not perform the namespace processing.
 *
 * "namespace-declarations"
 *
 *       true
 *               [required] (default)
 *               Include namespace declaration attributes, specified
 *               or  defaulted  from  the  schema or the DTD, in the
 *               document.  See  also  the  section  Declaring
 *               Namespaces in [XML Namespaces].
 *
 *       false
 *               [required]
 *               Discard  all  namespace declaration attributes. The
 *               Namespace   prefixes  are  retained  even  if  this
 *               feature is set to false.
 *
 * "normalize-characters"
 *
 *       true
 *               [optional]
 *               Perform   the   W3C   Text   Normalization  of  the
 *               characters [CharModel] in the document.
 *
 *       false
 *               [required] (default)
 *               Do not perform character normalization.
 *
 * "split-cdata-sections"
 *
 *       true
 *               [required] (default)
 *               Split  CDATA  sections containing the CDATA section
 *               termination  marker  ']]>'. When a CDATA section is
 *               split a warning is issued.
 *
 *       false
 *               [required]
 *               Signal an error if a CDATASection contains an
 *               unrepresentable character.
 *
 * "validate"
 *
 *       true
 *               [optional]
 *               Require  the  validation against a schema (i.e. XML
 *               schema,  DTD,  any  other type or representation of
 *               schema)  of  the document as it is being normalized
 *               as defined by [XML 1.0]. If validation errors
 *               are  found,  or  no  schema  was  found,  the error
 *               handler  is  notified.  Note  also  that normalized
 *               values  will  not  be exposed to the schema in used
 *               unless the feature datatype-normalization is true.
 *
 *               Note:  validate-if-schema and validate are mutually
 *               exclusive, setting one of them to true will set the
 *               other one to false.
 *
 *       false
 *               [required] (default)
 *               Only  XML  1.0  non-validating  processing  must be
 *               done.  Note  that  validation might still happen if
 *               validate-if-schema is true.
 *
 * "validate-if-schema"
 *
 *       true
 *               [optional]
 *               Enable  validation  only  if  a declaration for the
 *               document  element  can  be  found (independently of
 *               where  it  is  found,  i.e. XML schema, DTD, or any
 *               other   type   or  representation  of  schema).  If
 *               validation  errors  are found, the error handler is
 *               notified. Note also that normalized values will not
 *               be exposed to the schema in used unless the feature
 *               datatype-normalization is true.
 *
 *               Note:  validate-if-schema and validate are mutually
 *               exclusive, setting one of them to true will set the
 *               other one to false.
 *
 *       false
 *               [required] (default)
 *               No  validation  should be performed if the document
 *               has  a  schema.  Note  that  validation  must still
 *               happen if validate is true.
 *
 * "whitespace-in-element-content"
 *
 *       true
 *               [required] (default)
 *               Keep all white spaces in the document.
 *
 *             Issue normalizationFeature-15:
 *                     How   does   this   feature   interact   with
 *                     "validate" and
 *                     Text.isWhitespaceInElementContent.
 *                     Resolution:  issue  no  longer  relevant (f2f
 *                     october 2002).
 *
 *       false
 *               [optional]
 *               Discard   white  space  in  element  content  while
 *               normalizing.  The implementation is expected to use
 *               the isWhitespaceInElementContent flag on Text
 *               nodes to determine if a text node should be written
 *               out or not.
 *
 *   The  resolutions  of  entities  is done using Document.baseURI.
 *   However,  when  the  features "LS-Load" or "LS-Save" defined in
 *   [DOM  Level  3  Load  and  Save] are supported by the DOM
 *   implementation,  the  parameter  "entity-resolver"  can also be
 *   used  on  DOMConfiguration  objects  attached to Document
 *   nodes. If this parameter is set,
 *   Document.normalizeDocument   will   invoke   the   entity
 *   resolver instead of using Document.baseURI.
 */
class CDOM_EXPORT DOMConfiguration
{
protected:
    //-----------------------------------------------------------------------------------
    //  Constructor
    //-----------------------------------------------------------------------------------
    /** @name Hidden constructors */
    //@{
    DOMConfiguration() {};
    //@}

private:
    // -----------------------------------------------------------------------
    // Unimplemented constructors and operators
    // -----------------------------------------------------------------------
    /** @name Unimplemented constructors and operators */
    //@{
    DOMConfiguration(const DOMConfiguration &);
    DOMConfiguration & operator = (const DOMConfiguration &);
    //@}

public:

    // -----------------------------------------------------------------------
    //  Setter methods
    // -----------------------------------------------------------------------
    
    /** Set the value of a parameter. 
     * @param name The name of the parameter to set.
     * @param value The new value or null if the user wishes to unset the 
     * parameter. While the type of the value parameter is defined as 
     * <code>DOMUserData</code>, the object type must match the type defined
     * by the definition of the parameter. For example, if the parameter is 
     * "error-handler", the value must be of type <code>DOMErrorHandler</code>
     * @exception DOMException (NOT_SUPPORTED_ERR) Raised when the 
     * parameter name is recognized but the requested value cannot be set.
     * @exception DOMException (NOT_FOUND_ERR) Raised when the 
     * parameter name is not recognized.
     * @since DOM level 3
     **/
    virtual void setParameter(const XMLCh* name, const void* value) = 0;

    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    /** Return the value of a parameter if known. 
     * @param name The name of the parameter.
     * @return The current object associated with the specified parameter or 
     * null if no object has been associated or if the parameter is not 
     * supported.
     * @exception DOMException (NOT_FOUND_ERR) Raised when the i
     * boolean parameter 
     * name is not recognized.
     * @since DOM level 3
     **/    
    virtual const void* getParameter(const XMLCh* name) const = 0;

                                        
    // -----------------------------------------------------------------------
    //  Query methods
    // -----------------------------------------------------------------------

    /** Check if setting a parameter to a specific value is supported. 
     * @param name The name of the parameter to check.
     * @param value An object. if null, the returned value is true.
     * @return true if the parameter could be successfully set to the specified 
     * value, or false if the parameter is not recognized or the requested value 
     * is not supported. This does not change the current value of the parameter 
     * itself.
     * @since DOM level 3
     **/
    virtual bool canSetParameter(const XMLCh* name, const void* value) const = 0;

    // -----------------------------------------------------------------------
    //  All constructors are hidden, just the destructor is available
    // -----------------------------------------------------------------------
    /** @name Destructor */
    //@{
    /**
     * Destructor
     *
     */
    virtual ~DOMConfiguration() {};
    //@}
};
	
XERCES_CPP_NAMESPACE_END

#endif 

/**
 * End of file DOMConfiguration.hpp
 */
