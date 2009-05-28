/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef _TREE_H_
#define _TREE_H_

#include <stdio.h>

/* element */
typedef int        *Element;

/* attribute */
typedef int        *Attribute;

#include "document.h"

/* element type */
typedef struct _ElementType
  {
     SSchema             ElSSchema;
     int                 ElTypeNum;
  }
ElementType;

/* attribute type */
typedef struct _AttributeType
  {
     SSchema             AttrSSchema;
     int                 AttrTypeNum;
  }
AttributeType;

/* search domain */
typedef enum _SearchDomain
  {
     SearchForward, SearchInTree, SearchBackward
  }
SearchDomain;

/* element construct */
typedef enum _ConstructType
  {
     ConstructIdentity,
     ConstructList,
     ConstructChoice,
     ConstructOrderedAggregate,
     ConstructUnorderedAggregate,
     ConstructConstant,
     ConstructReference,
     ConstructBasicType,
     ConstructNature,
     ConstructPair,
     ConstructDocument,
     ConstructAny,
     ConstructEmpty,
     ConstructError
  }
ConstructType;

#ifndef __CEXTRACT__

/* ----------------------------------------------------------------------
   TtaChangeElementType
 
   Change the type of a given element.
   CAUTION: THIS FUNCTION SHOULD BE USED VERY CARFULLY!
   Parameters:
   element: the concerned element
   typeNum: new type for the element
   ---------------------------------------------------------------------- */
extern void TtaChangeElementType (Element element, int typeNum);

/* ----------------------------------------------------------------------
   TtaUpdateRootElementType
   Change the document and the root element schema if different to the
   requested schema.
   CAUTION: THIS FUNCTION SHOULD BE USED VERY CARFULLY!
   Parameters:
   root:   the concerned element must be the root of the document
   schemaName: the requested schema name.
   ---------------------------------------------------------------------- */
extern void TtaUpdateRootElementType (Element element, const char *schemaName, Document doc);

/*----------------------------------------------------------------------
   TtaChangeTypeOfElement
   Change the type of element elem into newTypeNum
   Parameters:
   element: the changed element
   document: the document for which the element is changed.
   newTypeNum: new type for the element
   Return value:
   TRUE if the operation is done.
 -----------------------------------------------------------------------*/
extern ThotBool TtaChangeTypeOfElement (Element elem, Document doc,
					int newTypeNum);

/* ----------------------------------------------------------------------
   TtaNewElement

   Creates a new element of a given type.
   Parameters:
   document: the document for which the element is created.
   elementType: type of the element to be created.
   Return value:
   the created element.
   ---------------------------------------------------------------------- */
extern Element TtaNewElement (Document document, ElementType elementType);

/* ----------------------------------------------------------------------
   TtaNewTree

   Creates a new element of a given type and all its descendants, according
   to the structure schema.
   Parameters:
   document: the document for which the tree is created.
   elementType: type of the root element of the tree to be created.
   label: label of the root element to be created. Empty string if the value
   of the label is undefined.
   Return value:
   the root element of the created tree.
   ---------------------------------------------------------------------- */
extern Element TtaNewTree (Document document, ElementType elementType,
			   const char* label);

/* ----------------------------------------------------------------------
   TtaNewTranscludedElement

   Creates a new element that is a dynamic copy of another element.
   Parameters:
   document: the document for which the element is created.
   orig:     the element that is copied.
   Return value:
   the created element.
   ---------------------------------------------------------------------- */
extern Element TtaNewTranscludedElement (Document document, Element orig);

/* ----------------------------------------------------------------------
   TtaCopyTree

   Creates a deep copy of a tree.
   Parameters:
   sourceElement: root of the tree to be copied.
   sourceDocument: the document containing the tree to be copied.
   destinationDocument: the document for which the copy must be created.
   parent: element that will become the parent of the created tree.
   Return value:
   the root element of the created tree.
   ---------------------------------------------------------------------- */
extern Element TtaCopyTree (Element sourceElement, Document sourceDocument,
			    Document destinationDocument, Element parent);

/* ----------------------------------------------------------------------
   TtaCopyElement

   Creates a copy of an element (does not copy the descendants).
   Parameters:
   sourceElement: element to be copied.
   sourceDocument: the document containing the element to be copied.
   destinationDocument: the document for which the copy must be created.
   parent: element that will become the parent of the created element.
   Return value:
   An element whic is a copy of the sourceElement.
   ---------------------------------------------------------------------- */
extern Element TtaCopyElement (Element sourceElement, Document sourceDocument,
		               Document destinationDocument, Element parent);

/* ----------------------------------------------------------------------
   TtaCopyGradient
   Copy the gradient attached to sourceElement as a gradient attached to the
   destinationElement (copy also all gradientStop elements)
   Parameters:
   sourceElement: element from which the gradient is to be copied.
   destinationElement: element that will receive the copy of the gradient.
   No return value.
   ---------------------------------------------------------------------- */
void TtaCopyGradient (Element sourceElement, Element destinationElement);

/* ----------------------------------------------------------------------
   TtaCreateDescent

   Creates a new element of a given type and inserts it in the tree as a
   descendant of a given element. All elements of the descent required by the
   structure schema are also created.
   Parameters:
   document: the document for which the tree is created.
   element: the element for which a descent will be created.
   elementType: type of the element to be created as the last descendant.
   Return value:
   the last descendant created or NULL if the element cannot be created.
   This element is empty.
   ---------------------------------------------------------------------- */
extern Element TtaCreateDescent (Document document, Element element,
				 ElementType elementType);

/* ----------------------------------------------------------------------
   TtaCreateDescentWithContent

   Creates a new element of a given type and inserts it in the tree as a
   descendant of a given element. All elements of the descent required by the
   structure schema are created, as well as the content of the requested element.
   Parameters:
   document: the document for which the tree is created.
   element: the element for which a descent will be created.
   elementType: type of the element to be created as the last descendant.
   Return value:
   the last descendant created or NULL if the element cannot be created.
   If not NULL, the minimum content of that element has been created.
   ---------------------------------------------------------------------- */
extern Element TtaCreateDescentWithContent (Document document,Element element,
					    ElementType elementType);

/* ----------------------------------------------------------------------
   TtaDeleteTree

   Deletes a tree (or a single element) and frees it.
   All references that points at any element of that tree are
   cancelled.
   The deleted element must not be used later.
   Parameters:
   element: the element (or root of the tree) to be deleted.
   document: the document containing the element to be deleted.
   ---------------------------------------------------------------------- */
extern void TtaDeleteTree (Element element, Document document);

/* ----------------------------------------------------------------------
   TtaAttachNewTree

   Attaches an entire tree (main tree or associated tree) to a document.
   Parameter:
   tree: root of the tree to be attached. This tree
   must be a valid main tree or associated tree according to the
   document structure schema.
   document: the document to which the tree is to be attached.
   ---------------------------------------------------------------------- */
extern void TtaAttachNewTree (Element tree, Document document);

/*----------------------------------------------------------------------
   TtaExportTree
 
   Saves an abstract tree into a file in a particular format. The output
   format is specified by a translation schema.
   Parameters:
   element: the root of the tree to be exported.
   document: the document containing the tree to be exported.
   fileName: name of the file in which the tree must be saved,
   including the directory name.
   TSchemaName: name of the translation schema to be used. The directory
   name must not be specified in parameter TSchemaName. See
   function TtaSetSchemaPath.
  ----------------------------------------------------------------------*/
extern void TtaExportTree (Element element, Document document,
          const char *fileName, const char *TSchemaName);
 
/* ----------------------------------------------------------------------
   TtaRedisplayElement
   Remove, recompute and redisplay the whole image of element element.
   ---------------------------------------------------------------------- */
extern void TtaRedisplayElement (Element element, Document document);

/* ----------------------------------------------------------------------
   TtaInsertSibling

   Inserts an element in a tree, as an immediate sibling of a given element.
   The element to be inserted must not yet be part of a document.
   Parameters:
   newElement: the element (or root of the tree) to be inserted.
   sibling: an element belonging to a tree. This element
   must not be the root of a tree.
   before: if TRUE, inserts newElement as previous sibling of sibling,
   if FALSE, inserts newElement as next sibling of sibling.
   document: the document to which both elements belong.
   ---------------------------------------------------------------------- */
extern void TtaInsertSibling (Element newElement, Element sibling,
			      ThotBool before, Document document);

/* ----------------------------------------------------------------------
   TtaInsertFirstChild

   Inserts an element in a tree, as the first child of a given element.
   The element to be inserted must not yet be part of a document.
   This function can also be used for attaching an option to a choice.
   Parameters:
   newElement: the element (or root of the tree) to be inserted.
   parent: an element belonging to a tree.
   document: the document to which both elements belong.
   Return parameter:
   If newElement is an option that replaces the choice, newElement takes
   the value of parent.
   ---------------------------------------------------------------------- */
extern void TtaInsertFirstChild (/*INOUT*/ Element *newElement, Element parent,
				 Document document);

/* ----------------------------------------------------------------------
   TtaCreateElement

   Create an element of a given type and insert it at the current position within
   a given document. The current position is defined by the current selection.
   If the current position is a single position (insertion point) the new element
   is simply inserted at that position. If one or several characters and/or
   elements are selected, the new element is created at that position and the
   selected characters/elements become the content of the new element, provided
   the  structure schema allows it.
   Parameters:
   elementType: type of the element to be created.
   document: the document for which the element is created.
   Return TRUE if the action is done
   ---------------------------------------------------------------------- */
extern ThotBool TtaCreateElement (ElementType elementType, Document document);

/* ----------------------------------------------------------------------
   TtaInsertElement

   Create an element of a given type and insert it at the current position within
   a given document. The current position is defined by the current selection.
   If the current position is a single position (insertion point) the new element
   is simply inserted at that position. If one or several characters and/or
   elements are selected, the new element is created before the first selected
   character/element and the selected characters/elements are not changed.
   Parameters:
   elementType: type of the element to be created.
   document: the document for which the element is created.
   Return TRUE if the action is done
   ---------------------------------------------------------------------- */
extern ThotBool TtaInsertElement (ElementType elementType, Document document);

/* ----------------------------------------------------------------------
   TtaInsertAnyElement

   Create an element whose type is defined in the structure schema and insert
   it at the current position within a given document. The current position is
   defined by the current selection.
   If the current position is a single position (insertion point) the new
   element is simply inserted at that position. If one or several characters
   and/or elements are selected, the new element is created before the first
   selected character/element if before is TRUE, after the last selected
   character/element if before is FALSE.
   The selection is set within the new element.
   Parameters:
   document: the document for which the element is created.
   ---------------------------------------------------------------------- */
extern void TtaInsertAnyElement (Document document, ThotBool before);

/* ----------------------------------------------------------------------
   TtaRemoveTree

   Removes a tree (or a single element) from its tree, without freeing it.
   Parameters:
   element: the element (or root of the tree) to be removed.
   document: the document containing the element to be removed.
   ---------------------------------------------------------------------- */
extern void TtaRemoveTree (Element element, Document document);

/* ----------------------------------------------------------------------
   TtaSetElementLineNumber
 
   Set the line number of a given element.
   Parameter:
   element: the element.
   nb: line number of the element.
   ---------------------------------------------------------------------- */
extern void TtaSetElementLineNumber (Element element, int nb);

/* ----------------------------------------------------------------------
   TtaSetAccessRight

   Sets the access rights for a given element.  Access rights apply only during
   the current editing session; they are not saved with the document. They must
   be set each time the document is loaded.
   Parameters:
   element: the element.
   right: access right for that element (ReadOnly, ReadWrite, Hidden).
   document: the document to which the element belongs.
   ---------------------------------------------------------------------- */
extern void TtaSetAccessRight (Element element, AccessRight right, Document document);

/* ----------------------------------------------------------------------
   TtaHolophrastElement

   Changes the holophrasting status of a given element.
   Parameters:
   element: the element.
   holophrast: TRUE: the element gets holophrasted if it is not,
   FALSE: if the element is holphrasted, it gets expanded.
   document: the document to which the element belongs.
   ---------------------------------------------------------------------- */
extern void TtaHolophrastElement (Element element, ThotBool holophrast,
				  Document document);

/* ----------------------------------------------------------------------
   TtaSetMandatoryInsertion

   Activate or disable element and attributes insertion. When a
   modification of the abstract tree is performed, mandatory elements
   and attributes are not inserted. 
   By default, insertion is activated.
   Parameter:
   on: 0 disables the insertion. All other values activates it.
   document: the document for which insertion is changed.
   ---------------------------------------------------------------------- */
extern void TtaSetMandatoryInsertion (ThotBool on, Document document);

/* ----------------------------------------------------------------------
   TtaSetStructureChecking

   Activate or disable structure checking. When structure checking is
   activated, modifications of the abstract tree are refused if they
   lead to an invalid structure with respect to the structure schema.
   By default, checking is activated.
   Parameter:
   on: 0 disables structure checking. All other values activates
   structure checking.
   document: the document for which structure checking is changed.
   ---------------------------------------------------------------------- */
extern void TtaSetStructureChecking (ThotBool on, Document document);

/* ----------------------------------------------------------------------
   TtaGetStructureChecking

   Return the current structure checking mode for a given document.
   When structure checking is activated, modifications of the abstract tree
   are refused if they lead to an invalid structure with respect to the
   structure schema. By default, checking is activated.
   Parameter:
   document: the document for which structure checking is asked.
   ---------------------------------------------------------------------- */
extern ThotBool TtaGetStructureChecking (Document document);

/* ----------------------------------------------------------------------
   TtaSetCheckingMode

   Changes checking mode.
   Parameter:
   strict: if TRUE, the presence of all mandatory elements is checked.
   ---------------------------------------------------------------------- */
extern void TtaSetCheckingMode (ThotBool strict);

/* ----------------------------------------------------------------------
   TtaGetMainRoot

   Returns the document element of the abstract tree representing a document.
   Parameter:
   document: the document.
   Return value:
   the document element of the abstract tree.
   ---------------------------------------------------------------------- */
extern Element TtaGetMainRoot (Document document);

/* ----------------------------------------------------------------------
   TtaGetRootElement

   Returns the root element of the abstract tree representing a document.
   Parameter:
   document: the document.
   Return value:
   the root element of the abstract tree.
   ---------------------------------------------------------------------- */
extern Element TtaGetRootElement (Document document);

/* ----------------------------------------------------------------------
   TtaNextAssociatedRoot

   Returns the root element of the associated tree that follows the
   tree to which a given element belongs.
   Parameters:
   document: the document.
   root: the element for which the next associated tree is searched.
   That element does not need to be the root of a tree.
   If root is NULL or if root is an element in the main tree, the
   root of the first associated tree is returned.
   Return parameter:
   root: the root element of the next associated tree.
   NULL if there is no next associated tree for the document.
   ---------------------------------------------------------------------- */
extern void TtaNextAssociatedRoot (Document document, /*INOUT*/ Element *root);

/* ----------------------------------------------------------------------
   TtaGetFirstChild

   Returns the first child element of a given element.
   Parameter:
   parent: the element for which the first child element is asked.
   Return value:
   the first child element of parent; NULL if parent has no child.
   ---------------------------------------------------------------------- */
extern Element TtaGetFirstChild (Element parent);

/* ----------------------------------------------------------------------
   TtaGetLastChild

   Returns the last child element of a given element.
   Parameter:
   parent: the element for which the last child element is asked.
   Return value:
   the last child element of parent; NULL if parent has no child.
   ---------------------------------------------------------------------- */
extern Element TtaGetLastChild (Element parent);

/* ----------------------------------------------------------------------
   TtaGetFirstLeaf

   Returns the first leaf element of a given element.
   Parameter:
   parent: the element for which the first leaf element is asked.
   Return value:
   the first leaf element of parent; parent itself if it has no leaf
   ---------------------------------------------------------------------- */
extern Element TtaGetFirstLeaf (Element parent);

/* ----------------------------------------------------------------------
   TtaGetLastLeaf

   Returns the last leaf element of a given element.
   Parameter:
   parent: the element for which the last leaf element is asked.
   Return value:
   the last leaf element of parent; parent itself if it has no leaf.
   ---------------------------------------------------------------------- */
extern Element TtaGetLastLeaf (Element parent);

/* ----------------------------------------------------------------------
   TtaPreviousSibling

   Returns the previous sibling element of a given element.
   Parameter:
   element: the element whose previous sibling is asked.
   Return parameter:
   element: the previous sibling element, or NULL if there is no
   previous sibling.
   ---------------------------------------------------------------------- */
extern void TtaPreviousSibling (/*INOUT*/ Element *element);

/* ----------------------------------------------------------------------
   TtaNextSibling

   Returns the next sibling element of a given element.
   Parameter:
   element: the element whose next sibling is asked.
   Return parameter:
   element: the next sibling element, or NULL if there is no next sibling.
   ---------------------------------------------------------------------- */
extern void TtaNextSibling (/*INOUT*/ Element *element);

/* ----------------------------------------------------------------------
   TtaGetSuccessor

   Returns the element that follows a given element at the same level or
   at the first upper level where there is a following element.
   Parameter:
   element: the element whose successor is asked.
   Return value:
   the successor, or NULL if there is no successor.
   ---------------------------------------------------------------------- */
extern Element TtaGetSuccessor (Element element);

/* ----------------------------------------------------------------------
   TtaGetPredecessor

   Returns the element that precedes a given element at the same level or
   at the first upper level where there is a preceding element.
   Parameter:
   element: the element whose predecessor is asked.
   Return value:
   the predecessor, or NULL if there is no predecessor.
   ---------------------------------------------------------------------- */
extern Element TtaGetPredecessor (Element element);

/* ----------------------------------------------------------------------
   TtaGetParent

   Returns the parent element (i.e. first ancestor) of a given element.
   Parameter:
   element: the element whose the parent is asked.
   Return value:
   the parent element, or NULL if there is no parent (root).
   ---------------------------------------------------------------------- */
extern Element TtaGetParent (Element element);

/* ----------------------------------------------------------------------
   TtaGetCommonAncestor

   Returns the common ancestor element of two given elements.
   Parameters:
   element1: the first element whose ancestor is asked.
   element2: the second element whose ancestor is asked.
   Return value:
   the common ancestor, or NULL if there is no
   common ancestor.
   ---------------------------------------------------------------------- */
extern Element TtaGetCommonAncestor (Element element1, Element element2);

/* ----------------------------------------------------------------------
   TtaGetTypedAncestor

   Returns the first ancestor of a given type for a given element.
   Parameters:
   element: the element whose ancestor is asked.
   ancestorType: type of the asked ancestor.
   Return value:
   the ancestor, or NULL if there is no ancestor of that type.
   ---------------------------------------------------------------------- */
extern Element TtaGetTypedAncestor (Element element, ElementType ancestorType);


/* ----------------------------------------------------------------------
   TtaGetExactTypedAncestor
   Returns the first ancestor of the exact given type for a given element.
   Parameters:
   element: the element whose ancestor is asked.
   ancestorType: type of the asked ancestor.
   Return value:
   the ancestor, or NULL if there is no ancestor of that type.
   ---------------------------------------------------------------------- */
extern Element TtaGetExactTypedAncestor (Element element, ElementType ancestorType);

/* ----------------------------------------------------------------------
   TtaGetElementType

   Returns the type of a given element.
   Parameter:
   element: the element.
   Return value:
   type of the element.
   ---------------------------------------------------------------------- */
extern ElementType TtaGetElementType (Element element);

/* ----------------------------------------------------------------------
   TtaIsExtensionElement

   Returns true if the element is from an extension schema
   Parameter:
   element: the element.
   Return value:
   true or false.
   ---------------------------------------------------------------------- */
extern ThotBool TtaIsExtensionElement (Element element);

/* ----------------------------------------------------------------------
   TtaIsTranscludedElement

   Returns true if the element is a transcluded element
   Parameter:
   element: the element.
   Return value:
   true or false.
   ---------------------------------------------------------------------- */
extern ThotBool     TtaIsTranscludedElement (Element element);

/* ----------------------------------------------------------------------
   TtaGetElementTypeName

   Returns the name of an element type.
   Parameter:
   elementType: element type.
   Return value:
   name of that type.
   ---------------------------------------------------------------------- */
extern char *TtaGetElementTypeName (ElementType elementType);

/* ----------------------------------------------------------------------
   TtaGetElementTypeOriginalName

   Returns the name of an element type in the language it is defined in
   the structure schema.
   Parameter:
   elementType: element type.
   Return value:
   original name of that type.
   ---------------------------------------------------------------------- */
extern char *TtaGetElementTypeOriginalName (ElementType elementType);

/* ----------------------------------------------------------------------
   TtaGiveTypeFromName

   Gives an element type whose name is known (the structure schema that
   defines that type must be loaded). That type is searched in a given
   structure schema (elementType.ElSSchema) and in all structure schemas
   that are extensions of that structure schema or natures used in that
   structure schema.
   Parameters:
   elementType.ElSSchema: the structure schema of interest.
   name: the name of the type of interest.
   Return parameter:
   elementType: the type having this name, or elementType.ElTypeNum = 0
   if the type is not found.
   ---------------------------------------------------------------------- */
extern void TtaGiveTypeFromName (/*OUT*/ ElementType * elementType, char *name);

/* ----------------------------------------------------------------------
   TtaGiveTypeFromOriginalName

   Gives an element type whose name is known (the structure schema that
   defines that type must be loaded). That type is searched in a given
   structure schema (elementType.ElSSchema) and in all structure schemas
   that are extensions of that structure schema or natures used in that
   structure schema.
   Parameters:
   elementType.ElSSchema: the structure schema of interest.
   name: the name of the type of interest in the language it is defined
   in the structure schema.
   Return parameter:
   elementType: the type having this name, or elementType.ElTypeNum = 0
   if the type is not found.
   ---------------------------------------------------------------------- */
extern void TtaGiveTypeFromOriginalName (/*OUT*/ ElementType *elementType,
					 char *name);

/* ----------------------------------------------------------------------
   TtaSameTypes

   Compares two element types.
   Parameters:
   type1: first element type.
   type2: second element type.
   Return value:
   0 if both types are different, 1 if they are identical.
   ---------------------------------------------------------------------- */
extern ThotBool TtaSameTypes (ElementType type1, ElementType type2);

/* ----------------------------------------------------------------------
   TtaGetElementLabel

   Returns the label of a given element.
   Parameter:
   element: the element.
   Return value:
   label of the element.
   ---------------------------------------------------------------------- */
extern char *TtaGetElementLabel (Element element);

/* ----------------------------------------------------------------------
   TtaGetElementLineNumber
 
   Returns the line number of a given element.
   Parameter:
   element: the element.
   Return value:
   line number of the element.
   ---------------------------------------------------------------------- */
extern int TtaGetElementLineNumber (Element element);

/* ----------------------------------------------------------------------
   TtaGetElementLevel

   Returns the level of an element in the abstract tree. The root has
   level 0, its children have level 1, its grandchildren have level 2,
   and so on
   Parameter:
   element: the element.
   Return value:
   level of the element.
   ---------------------------------------------------------------------- */
extern int TtaGetElementLevel (Element element);

/* ----------------------------------------------------------------------
   TtaGetElementVolume

   Returns the volume of a given element, i.e. the number of characters
   contained in that element.
   Parameter:
   element: the element.
   Return value:
   element volume.
   ---------------------------------------------------------------------- */
extern int TtaGetElementVolume (Element element);

/* ----------------------------------------------------------------------
   TtaIsConstant

   Indicates whether an element type is a constant.
   Parameter:
   elementType: type to be tested.
   Return value:
   1 = the type is a constant, 0 = the type is not a constant.
   ---------------------------------------------------------------------- */
extern int TtaIsConstant (ElementType elementType);

/* ----------------------------------------------------------------------
   TtaIsLeaf

   Indicates whether an element type is a leaf.
   Parameter:
   elementType: type to be tested.
   Return value:
   1 if the type is a leaf, 0 if the type is not a leaf.
   ---------------------------------------------------------------------- */
extern ThotBool TtaIsLeaf (ElementType elementType);

/* ----------------------------------------------------------------------
   TtaGetConstructOfType

   Returns the construct of an element type
   Parameter:
   elementType: the element type of interest.
   Return value:
   the construct that defines the structure of that element type.
   ---------------------------------------------------------------------- */
extern ConstructType TtaGetConstructOfType (ElementType elementType);

/* ----------------------------------------------------------------------
   TtaGetCardinalOfType

   Returns the cardinal of an element type, e.g. the number of types
   that participates in its definition in the structure schema.
   Parameter:
   elementType: the element type of interest.
   Return value:
   the cardinal of that element type (integer value).
   ---------------------------------------------------------------------- */
extern int TtaGetCardinalOfType(ElementType elementType);

/* ----------------------------------------------------------------------
   TtaGiveConstructorsOfType

   Fills a array with the element types defining the given element type
   in the structure schema.
   Parameter:
   typesArray: a pointer to an initialized ElementType array.
   size: size of the array
   elementType: the element type of interest.
   Return value:
   typesArray: a array of element types.
   size: the number of types actually inserted in the array
   ---------------------------------------------------------------------- */
extern void TtaGiveConstructorsOfType(/*INOUT*/ ElementType **typesArrey,
				      /*INOUT*/  int *size, ElementType elementType);

/* ----------------------------------------------------------------------
   TtaGetRankInAggregate

   Returns the rank that an element of type componentType should have in an
   aggregate of type aggregateType, according to the structure schema.
   Parameter:
   componentType: type of the element whose rank is asked.
   elementType: type of the aggregate.
   Return value:
   rank of the component (first component = 1), or 0 if no element of type
   componentType is allowed in the aggregate or if aggregateType is not
   an aggregate.
   ---------------------------------------------------------------------- */
extern int TtaGetRankInAggregate (ElementType componentType, ElementType aggregateType);

/* ----------------------------------------------------------------------
   TtaIsOptionalInAggregate

   Returns TRUE if component of rank rank is declared optionnal in 
   the aggregate of type elementType, according to the structure schema.
   Parameter:
   rank: the rank in the agreggate declaration of the component to be tested.
   elementType: type of the aggregate.
   Return value:
   TRUE if this component is optional.
   ---------------------------------------------------------------------- */
extern ThotBool TtaIsOptionalInAggregate(int rank, ElementType elementType);

/* ----------------------------------------------------------------------
   TtaGetConstruct

   Returns the construct of an element.
   Parameter:
   element: the element of interest.
   Return value:
   the construct that defines the structure of that element.
   ---------------------------------------------------------------------- */
extern ConstructType TtaGetConstruct (Element element);

/* ----------------------------------------------------------------------
   TtaGetAccessRight

   Returns the access rights for a given element.
   Parameter:
   element: the element.
   Return Value:
   access right for that element (ReadOnly, ReadWrite, Hidden, Inherited).
   ---------------------------------------------------------------------- */
extern AccessRight TtaGetAccessRight (Element element);

/* ----------------------------------------------------------------------
   TtaIsHolophrasted

   Tests whether a given element is holphrasted or not.
   Parameter:
   element: the element to be tested.
   Return Value:
   1 if the element is holphrasted, 0 if not.
   ---------------------------------------------------------------------- */
extern int TtaIsHolophrasted (Element element);

/* ----------------------------------------------------------------------
   TtaIsReadOnly

   Tests whether a given element is protected against user modifications (ReadOnly).
   Parameter:
   element: the element to be tested.
   Return Value:
   1 if the element is protected, 0 if not.
   ---------------------------------------------------------------------- */
extern int TtaIsReadOnly (Element element);

/* ----------------------------------------------------------------------
   TtaIsSetReadOnly
   Tests whether the given element itself is ReadOnly.
   Parameter:
   element: the element to be tested.
   Return Value:
   1 if the element is protected, 0 if not.
   ---------------------------------------------------------------------- */
extern int TtaIsSetReadOnly (Element element);

/* ----------------------------------------------------------------------
   TtaIsHidden

   Tests whether a given element is hidden to the user.
   Parameter:
   element: the element to be tested.
   Return Value:
   1 if the element is hidden, 0 if not.
   ---------------------------------------------------------------------- */
extern int TtaIsHidden (Element element);

/* ----------------------------------------------------------------------
   TtaIsCopy
   Tests whether a given element is a copy.
   Parameter:
   element: the element to be tested.
   Return Value:
   1 if the element is a copy, 0 if not.
   ---------------------------------------------------------------------- */
extern int TtaIsCopy (Element element);


/* ----------------------------------------------------------------------
   TtaIsGaphics
   Tests whether a given element is a graphics.
   Parameter:
   element: the element to be tested.
   Return Value:
   1 if the element is a graphics, 0 if not.

   ---------------------------------------------------------------------- */
extern int TtaIsGraphics (Element element);

/* ----------------------------------------------------------------------
   TtaIsInAnInclusion

   Tests whether a given element is (in) an included element. An included element
   is a "live" copy of another element.
   Parameter:
   element: the element to be tested.
   Return Value:
   1 if the element is included, 0 if not.
   ---------------------------------------------------------------------- */
extern int TtaIsInAnInclusion (Element element);

/* ----------------------------------------------------------------------
   TtaIsAncestor

   Tests if an element is an ancestor of another element.
   Parameters:
   element: an element.
   ancestor: the supposed ancestor of element.
   Return value:
   1 if ancestor in an ancestor of element, 0 if not.
   ---------------------------------------------------------------------- */
extern int TtaIsAncestor (Element element, Element ancestor);

/* ----------------------------------------------------------------------
   TtaIsBefore

   Tests if an element precedes another element in the preorder traversal
   of the tree.
   Parameters:
   element1: the first element.
   element2: the second element.
   Return value:
   1 if the first element precedes the second element, 0 if not.
   ---------------------------------------------------------------------- */
extern int TtaIsBefore (Element element1, Element element2);

/* ----------------------------------------------------------------------
   TtaIsFirstPairedElement

   Indicates if a given paired element is the first or the second of the pair.
   Parameter:
   element: the paired element.
   Return value:
   1 if it is the first element of the pair, 0 if it is the second.
   ---------------------------------------------------------------------- */
extern int TtaIsFirstPairedElement (Element element);

/* ----------------------------------------------------------------------
   TtaCanInsertSibling

   Checks whether an element of a given type can be inserted in an
   abstract tree as an immediate sibling of an existing element.
   Parameters:
   elementType: element type to be checked.
   sibling: an existing element which is part of an abstract tree.
   before: if TRUE, checks if insertion is allowed before element "sibling".
   If FALSE, checks if insertion is allowed after element "sibling".
   document: the document to which element "sibling" belongs.
   Return value:
   TRUE if that element type can be inserted, FALSE if the structure
   schema does not allow that insertion.
   ---------------------------------------------------------------------- */
extern ThotBool TtaCanInsertSibling (ElementType elementType, Element sibling,
				     ThotBool before, Document document);

/* ----------------------------------------------------------------------
   TtaCanInsertFirstChild

   Checks whether an element of a given type can be inserted in an
   abstract tree as the first child of an existing element (parent).
   Parameters:
   elementType: element type to be checked.
   parent: an existing element which is part of an abstract tree.
   document: the document to which element parent belongs.
   Return value:
   TRUE if that element type can be inserted, FALSE if the structure
   schema does not allow that insertion.
   ---------------------------------------------------------------------- */
extern ThotBool TtaCanInsertFirstChild (ElementType elementType,
					Element parent, Document document);

/* ----------------------------------------------------------------------
   TtaGetDocument

   Returns the document containing a given element
   Parameters:
   element: the element for which document is asked.
   Return value:
   the document containing that element or 0 if the element does not
   belong to any document.
   ---------------------------------------------------------------------- */
extern Document TtaGetDocument (Element element);

/* ----------------------------------------------------------------------
   TtaNextCopiedElement

   Returns one of the elements that have been copied into the ``clipboard''
   by the last Copy or Cut command. (This function is available only in the
   ThotEditor library).
   Parameter:
   element: NULL if the first element of the clipboard is asked;
   an element of the clipboard if the next one is asked.
   Return parameter:
   element: the asked element if it exists, or NULL if there is no next
   element in the clipboard or if the clipboard is empty.
   ---------------------------------------------------------------------- */
extern void TtaNextCopiedElement (/*OUT*/ Element *element);

/* ----------------------------------------------------------------------
   TtaGetCopiedDocument

   Returns the document from which the current content of the clipboard
   has been copied or cut. (This function is available only in the ThotEditor
   library).
   Parameters:
   No parameter.
   Return value:
   the document from which the current content of the clipboard has been
   copied or cut; 0 if the clipboard is empty.
   ---------------------------------------------------------------------- */
extern Document TtaGetCopiedDocument (void);

/* ----------------------------------------------------------------------
   TtaSearchTypedElement

   Returns the first element of a given type. Searching can be done in
   a tree or starting from a given element towards the beginning or the
   end of the abstract tree.
   Parameters:
   searchedType: type of element to be searched. If searchedType.ElSSchema
   is NULL, searchedType must be a basic type ; then the next basic
   element of that type will be returned, whatever its structure
   schema.
   scope: SearchForward, SearchBackward or SearchInTree.
   element: the element that is the root of the tree
   (if scope = SearchInTree) or the starting element
   (if scope = SearchForward or SearchBackward).
   Return value:
   the element found, or NULL if no element has been found.
   ---------------------------------------------------------------------- */
extern Element TtaSearchTypedElement (ElementType searchedType, SearchDomain scope,
				      Element element);

/* ----------------------------------------------------------------------
   TtaSearchTypedElementInTree

   Returns the first element of a given type. Searching can be done in
   a tree or starting from a given element towards the beginning or the
   end of the abstract tree.. In any case the returned element must be
   part of the parent tree.
   Parameters:
   searchedType: type of element to be searched. If searchedType.ElSSchema
   is NULL, searchedType must be a basic type ; then the next basic
   element of that type will be returned, whatever its structure
   schema.
   scope: SearchForward, SearchBackward or SearchInTree.
   element: the element that is the root of the tree
   (if scope = SearchInTree) or the starting element
   (if scope = SearchForward or SearchBackward).
   Return value:
   the element found, or NULL if no element has been found.
   ---------------------------------------------------------------------- */
extern Element TtaSearchTypedElementInTree (ElementType searchedType,
					    SearchDomain scope, Element parent,
					    Element element);

/* ----------------------------------------------------------------------
   TtaSearchElementByLabel

   Searches the element that has a given label. The search is done in
   a given tree.
   Parameters:
   searchedLabel: label of element to be searched.
   element: the element that is the root of the tree in which the search
   is done.
   Return value:
   the element found, or NULL if no element has been found.
   ---------------------------------------------------------------------- */
extern Element TtaSearchElementByLabel (char *searchedLabel, Element element);

/* ----------------------------------------------------------------------
   TtaSearchEmptyElement

   Searches the next empty element. An empty element is either a compound
   element without child or a leaf without content.
   Searching can be done in a tree or starting from a given element towards
   the beginning or the end of the abstract tree.
   Parameters:
   scope: SearchForward, SearchBackward or SearchInTree.
   element: the element that is the root of the tree
   (if scope = SearchInTree) or the starting element
   (if scope = SearchForward or SearchBackward).
   Return values:
   the element found, or NULL if not found.
   ---------------------------------------------------------------------- */
extern Element TtaSearchEmptyElement (SearchDomain scope, Element element);

/* ----------------------------------------------------------------------
   TtaSearchOtherPairedElement

   Returns the element that is part of the same pair as a given element.
   Parameter:
   element: the element whose paired element is searched.
   Return value:
   the paired element.
   ---------------------------------------------------------------------- */
extern Element TtaSearchOtherPairedElement (Element element);

/* ----------------------------------------------------------------------
   TtaSearchElementAmong5Types

   Returns the first element typed among the 5 types provided
   Searching can be done in a tree or starting from a given element
   towards the beginning of the abstract tree.
   Parameters:
   typeX: types of element to be searched. If typeX.ElSSchema
   is NULL, typeX must be a basic type ; then the next basic
   element of that type will be returned, whatever its structure
   schema.
   scope: SearchForward or SearchInTree.
   element: the element that is the root of the tree
   (if scope = SearchInTree) or the starting element
   (if scope = SearchForward).
   Return value:
   the element found, or NULL if no element has been found.
   ---------------------------------------------------------------------- */
extern Element TtaSearchElementAmong5Types (ElementType type1, 
					    ElementType type2,
					    ElementType type3,
					    ElementType type4,
					    ElementType type5, 
					    SearchDomain scope,
					    Element element);
/* ----------------------------------------------------------------------
   TtaBackSearchElementAmong5Types

   Returns the first element typed among the 5 types provided
   Searching can be done in the tree starting from a given element
   backward the beginning of the abstract tree.
   Parameters:
   typeX: types of element to be searched. If typeX.ElSSchema
   is NULL, typeX must be a basic type ; then the next basic
   element of that type will be returned, whatever its structure
   schema.
   scope: SearchForward or SearchInTree.
   element: the element that is the root of the tree
   (if scope = SearchInTree) or the starting element
   (if scope = SearchForward).
   Return value:
   the element found, or NULL if no element has been found.
   ---------------------------------------------------------------------- */
extern Element TtaBackSearchElementAmong5Types (ElementType type1, 
						ElementType type2,
						ElementType type3,
						ElementType type4,
						ElementType type5, 
						SearchDomain scope,
						Element element);

/* ----------------------------------------------------------------------
   TtaSearchNoPageBreak

   Returns the first sibling element that is not a page break.
   Parameter:
   element: the element.
   forward: TRUE for skipping the next page breaks, FALSE for skipping
   the previous ones.
   Return value:
   the first sibling element, or NULL if there are
   only page breaks.
   ---------------------------------------------------------------------- */
extern Element TtaSearchNoPageBreak (Element element, ThotBool forward);

/*----------------------------------------------------------------------
   TtaListAbstractTree

   Produces in a file a human-readable form of an abstract tree.
   Parameters:
   root: the root element of the tree to be listed.
   fileDescriptor: file descriptor of the file that will contain the list.
   This file must be open when calling the function.
  ----------------------------------------------------------------------*/
extern void TtaListAbstractTree (Element root, FILE *fileDescriptor);

/* ----------------------------------------------------------------------
   TtaHasHiddenException

   Parameters:
   elType : the element type we want to test.
   Returns TRUE if the elType is defined by the document schema's
   DTD. For example, elements with the hidden and exception
   attributes are not included in the DTD.
   ---------------------------------------------------------------------- */
extern ThotBool TtaHasHiddenException (ElementType elType);

/* ----------------------------------------------------------------------
   TtaHasNotElementException
   Returns TRUE if the elType has the ExcNotAnElementNode exception
   (Comments, PIs, etc)
   ---------------------------------------------------------------------- */
extern ThotBool TtaHasNotElementException (ElementType elType);

/* ----------------------------------------------------------------------
   TtaHasInvisibleException

   Returns TRUE if the elType has the CreateNL exception
   (Preformatted, STYLE_, SCRIPT_ andText_Area HTML elements
   ---------------------------------------------------------------------- */
extern ThotBool TtaHasReturnCreateNLException (ElementType elType);

/* ----------------------------------------------------------------------
   TtaHasInvisibleException

   Parameters:
   attrType : the attribute type we want to test.
   Returns TRUE if the attrType is defined by the document schema's
   DTD. For example, elements with the hidden and exception
   attributes are not included in the DTD.
   ---------------------------------------------------------------------- */
/* extern ThotBool TtaHasInvisibleException (AttributeType attrType); */

/* ----------------------------------------------------------------------
   TtaSearchElementBySchema

   Searches the first element that has a given schema.
   The search is done in a given tree.
   Parameters:
   searchedSschema: schema of element to be searched.
   element: the element that is the root of the tree in which the search
   is done.
   Return value:
   the element found, or NULL if no element has been found.
   ---------------------------------------------------------------------- */
extern Element TtaSearchElementBySchema (SSchema searchedSschema, Element element);

/*----------------------------------------------------------------------
  TtaAppendXmlAttribute
  Add a new xml global attribute
  ----------------------------------------------------------------------*/
extern void TtaAppendXmlAttribute (char *XMLName, AttributeType *attrType,
				   Document document);

/*----------------------------------------------------------------------
  TtaGetXmlAttributeType
  If attrType->AttrSSchema is not NULL, search in that specific schema,
  otherwise search in different schemas added to the document.
  ----------------------------------------------------------------------*/
extern void TtaGetXmlAttributeType (char *XMLName, AttributeType *attrType,
				   Document document);

/*----------------------------------------------------------------------
   TtaAddEmptyBox
   Add the specific presentation rule Createlast(EmptyBox) to an empty element
  ----------------------------------------------------------------------*/
extern void TtaAddEmptyBox (Element el);

/* ----------------------------------------------------------------------
   TtaIsElementWithSpacePreserve

   Returns TRUE if element white spaces should be preserved.
   ---------------------------------------------------------------------- */
extern ThotBool TtaIsElementWithSpacePreserve (Element el);

/*----------------------------------------------------------------------
   TtaHasXmlInLineRule
   Retuns TRUE if the element type has a 'Line' presentation rule
  ----------------------------------------------------------------------*/
extern ThotBool TtaHasXmlInLineRule (ElementType elType, Document document);

/*----------------------------------------------------------------------
   TtaSetXmlInLineRule
   Add a generic rule to an element type
  ----------------------------------------------------------------------*/
extern void TtaSetXmlInLineRule (ElementType elType, Document document);

/*----------------------------------------------------------------------
  TtaAppendXmlElement
  Add a new element to the schema
  ----------------------------------------------------------------------*/
extern void TtaAppendXmlElement (const char *XMLName, ElementType *elType,
				 char **mappedName, Document doc);

/*----------------------------------------------------------------------
  TtaGetXmlElementType
  Search in elType->ElSSchema if not NULL otherwise,
  search in the different loaded natures.
   ----------------------------------------------------------------------*/
extern void TtaGetXmlElementType (const char *XMLElementName, ElementType *elType,
				  char **mappedName, Document doc);

/*----------------------------------------------------------------------
  TtaChangeGenericSSchemaName
  Change the name of a generic xml schema
  ----------------------------------------------------------------------*/
extern void TtaChangeGenericSchemaNames (const char *sSchemauri,
            const char *sSchemaName, Document doc);


/*----------------------------------------------------------------------
  TtaBreakElement
  Split the pSplitEl element and complete to replicate the pElReplicate
  element.
  block is TRUE if the split element is a block
  Return TRUE if okay
  select: select the new element
  ----------------------------------------------------------------------*/
extern ThotBool TtaBreakElement (Element replicate, Element split,
                                 int splitIndex, ThotBool block, ThotBool select);
#endif /* __CEXTRACT__ */

#endif
