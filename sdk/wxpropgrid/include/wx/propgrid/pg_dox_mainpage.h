/////////////////////////////////////////////////////////////////////////////
// Name:        pg_dox_mainpage.h
// Purpose:     wxPropertyGrid Doxygen Documentation
// Author:      Jaakko Salli
// Modified by:
// Created:     Oct-08-2004
// RCS-ID:      $Id:
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __WX_PG_DOX_MAINPAGE_H__
#define __WX_PG_DOX_MAINPAGE_H__

/**
    @mainpage wxPropertyGrid 1.4.3 Overview

      wxPropertyGrid is a specialized for editing properties such as strings, numbers,
    flagsets, fonts, and colours. It allows hierarchial, collapsible properties (via
    so-called categories that can hold child properties), sub-properties, and has strong
    wxVariant support.

    <b>Documentation for wxPython bindings:</b> For a tutorial see the accompanied
    wxPython readme file and the test_propgrid.py sample. Otherwise, individual
    member functions should work very much the same as with the C++ wxWidgets,
    so you'll probably find wxPropertyGrid and wxPropertyGridManager class references handy.

    Key Classes:\n
      wxPropertyGrid\n
      wxPropertyGridManager\n
      wxPropertyGridEvent\n

    Header files:\n
      <b>wx/propgrid/propgrid.h:</b> Mandatory when using wxPropertyGrid.\n
      <b>wx/propgrid/advprops.h:</b> For less often used property classes.\n
      <b>wx/propgrid/manager.h:</b> Mandatory when using wxPropertyGridManager.\n
      <b>wx/propgrid/propdev.h:</b> Mandatory when implementing custom property classes.\n

    @ref whatsnew\n
    @ref basics\n
    @ref categories\n
    @ref parentprops\n
    @ref enumandflags\n
    @ref advprops\n
    @ref iterating\n
    @ref operations\n
    @ref events\n
    @ref validating\n
    @ref populating\n
    @ref cellrender\n
    @ref customizing\n
    @ref usage2\n
    @ref subclassing\n
    @ref misc\n
    @ref proplist\n
    @ref userhelp\n
    @ref notes\n
    @ref newprops\n
    @ref neweditors\n

    @section whatsnew What's new in wxPropertyGrid 1.4?

    For comprehensive list of changes and new features, please see the changelog.

    For people implementing custom property classes the differences are extensive,
    so if you are coming from wxPropertyGrid 1.2.x, it is recommended that you take a look.


    @section basics Creating and Populating wxPropertyGrid

    As seen here, wxPropertyGrid is constructed in the same way as
    other wxWidgets controls:

    @code

    // Necessary header file
    #include <wx/propgrid/propgrid.h>

    ...

        // Assumes code is in frame/dialog constructor

        // Construct wxPropertyGrid control
        wxPropertyGrid* pg = new wxPropertyGrid(
            this, // parent
            PGID, // id
            wxDefaultPosition, // position
            wxDefaultSize, // size
            // Some specific window styles - for all additional styles,
            // see Modules->PropertyGrid Window Styles
            wxPG_AUTO_SORT | // Automatic sorting after items added
            wxPG_SPLITTER_AUTO_CENTER | // Automatically center splitter until user manually adjusts it
            // Default style
            wxPG_DEFAULT_STYLE );

        // Window style flags are at premium, so some less often needed ones are
        // available as extra window styles (wxPG_EX_xxx) which must be set using
        // SetExtraStyle member function. wxPG_EX_HELP_AS_TOOLTIPS, for instance,
        // allows displaying help strings as tooltips.
        pg->SetExtraStyle( wxPG_EX_HELP_AS_TOOLTIPS );

    @endcode

      (for complete list of new window styles: @link wndflags Additional Window Styles@endlink)

      wxPropertyGrid is usually populated with lines like this:

    @code
        pg->Append( new wxStringProperty(wxT("Label"),wxT("Name"),wxT("Initial Value")) );
    @endcode

    Naturally, wxStringProperty is a property class. Only the first function argument (label)
    is mandatory. Second one, name, defaults to label and, third, the initial value, to
    default value. If constant wxPG_LABEL is used as the name argument, then the label is
    automatically used as a name as well (this is more efficient than manually
    defining both as the same). Use of empty name is discouraged and will sometimes result in
    run-time error. Note that all property class constructors have quite similar
    constructor argument list.

    To demonstrate other common property classes, here's another code snippet:

    @code

        // Add int property
        pg->Append( new wxIntProperty(wxT("IntProperty"), wxPG_LABEL, 12345678) );

        // Add float property (value type is actually double)
        pg->Append( new wxFloatProperty(wxT("FloatProperty"), wxPG_LABEL, 12345.678) );

        // Add a bool property
        pg->Append( new wxBoolProperty(wxT("BoolProperty"), wxPG_LABEL, false) );

        // A string property that can be edited in a separate editor dialog.
        pg->Append( new wxLongStringProperty(wxT("LongStringProperty"),
                                             wxPG_LABEL,
                                             wxT("This is much longer string than the ")
                                             wxT("first one. Edit it by clicking the button.")));

        // String editor with dir selector button.
        pg->Append( new wxDirProperty(wxT("DirProperty"), wxPG_LABEL, ::wxGetUserHome()) );

        // wxArrayStringProperty embeds a wxArrayString.
        pg->Append( new wxArrayStringProperty(wxT("Label of ArrayStringProperty"),
                                              wxT("NameOfArrayStringProp")));

        // A file selector property.
        pg->Append( new wxFileProperty(wxT("FileProperty"), wxPG_LABEL, wxEmptyString) );

        // Extra: set wildcard for file property (format same as in wxFileDialog).
        pg->SetPropertyAttribute( wxT("FileProperty"),
                                  wxPG_FILE_WILDCARD,
                                  wxT("All files (*.*)|*.*") );

    @endcode

      Operations on properties should be done either via wxPropertyGrid's
    (or wxPropertyGridManager's) methods, or by acquiring pointer to a property
    (Append returns a wxPGProperty* or wxPGId, which is typedef for same), and then
    calling its method. Note however that property's methods generally do not
    automatically update grid graphics.

      Property container functions operating on properties, such as SetPropertyValue or
    DisableProperty, all accept a special wxPGPropArg, argument which can automatically
    convert name of a property to a pointer. For instance:

    @code
        // A file selector property.
        wxPGPropety* p = pg->Append( new wxFileProperty(wxT("FileProperty"), wxPG_LABEL, wxEmptyString) );

        // Valid: Set wildcard by name
        pg->SetPropertyAttribute( wxT("FileProperty"),
                                  wxPG_FILE_WILDCARD,
                                  wxT("All files (*.*)|*.*") );

        // Also Valid: Set wildcard by ptr
        pg->SetPropertyAttribute( p,
                                  wxPG_FILE_WILDCARD,
                                  wxT("All files (*.*)|*.*") );
    @endcode

    Using pointer is faster, since it doesn't require hash map lookup. Anyway, you can allways
    get property pointer (wxPGProperty*) as Append/Insert return value, or by calling
    GetPropertyByName.

      Below are samples for using some of the more commong operations. See 
    wxPropertyGridInterface and wxPropertyGrid class references for complete list.

    @code

        // wxPGId is a short-hand for wxPGProperty*. Let's use it this time.
        wxPGId id = pg->GetPropertyByName( wxT("MyProperty") );

        // There are many overloaded versions of this method, of which each accept
        // different type of value.
        pg->SetPropertyValue( wxT("MyProperty"), 200 );

        // Setting a string works for all properties - conversion is done
        // automatically.
        pg->SetPropertyValue( id, wxT("400") );

        // Getting property value as wxVariant.
        wxVariant value = pg->GetPropertyValue( wxT("MyProperty") );

        // Getting property value as String (again, works for all typs).
        wxString value = pg->GetPropertyValueAsString( id );

        // Getting property value as int. Provokes a run-time error
        // if used with property which value type is not "long".
        long value = pg->GetPropertyValueAsLong( wxT("MyProperty") );

        // Set new name.
        pg->SetPropertyName( wxT("MyProperty"), wxT("X") );

        // Set new label - we need to use the new name.
        pg->SetPropertyLabel( wxT("X"), wxT("New Label") );

        // Disable the property. It's text will appear greyed.
        // This is probably the closest you can get if you want
        // a "read-only" property.
        pg->DisableProperty( id );

    @endcode


    @section categories Categories

      wxPropertyGrid has a hierarchial property storage and display model, which
    allows property categories to hold child properties and even other
    categories. Other than that, from the programmer's point of view, categories
    can be treated exactly the same as "other" properties. For example, despite
    its name, GetPropertyByName also returns a category by name, and SetPropertyLabel
    also sets label of a category. Note however that sometimes the label of a
    property category may be referred as caption (for example, there is
    SetCaptionForegroundColour method that sets text colour of a property category's label).

      When category is added at the top (i.e. root) level of the hierarchy,
    it becomes a *current category*. This means that all other (non-category)
    properties after it are automatically added to it. You may add
    properties to specific categories by using wxPropertyGrid::Insert or wxPropertyGrid::AppendIn.

      Category code sample:

    @code

        // One way to add category (similar to how other properties are added)
        pg->Append( new wxPropertyCategory(wxT("Main")) );

        // All these are added to "Main" category
        pg->Append( new wxStringProperty(wxT("Name")) );
        pg->Append( new wxIntProperty(wxT("Age"),wxPG_LABEL,25) );
        pg->Append( new wxIntProperty(wxT("Height"),wxPG_LABEL,180) );
        pg->Append( new wxIntProperty(wxT("Weight")) );

        // Another one
        pg->Append( new wxPropertyCategory(wxT("Attrikbutes")) );

        // All these are added to "Attributes" category
        pg->Append( new wxIntProperty(wxT("Intelligence")) );
        pg->Append( new wxIntProperty(wxT("Agility")) );
        pg->Append( new wxIntProperty(wxT("Strength")) );

    @endcode


    @section parentprops Tree-like Property Structure

      As a new feature in version 1.3.1, basicly any property can have children. There
    are few limitations, however.

    @remarks
    - Names of properties with non-category, non-root parents are not stored in hash map.
      Instead, they can be accessed with strings like "Parent.Child". For instance, in
      the sample below, child property named "Max. Speed (mph)" can be accessed by global
      name "Car.Speeds.Max Speed (mph)".
    - If you want to property's value to be a string composed based on the values of
      child properties, you must use wxStringProperty as parent and use value "<composed>".
    - Events (eg. change of value) that occur in parent do not propagate to children. Events
      that occur in children will propagate to parents, but only if they are wxStringProperties
      with "<composed>" value.
    - Old wxParentProperty class is deprecated, and remains as a typedef of wxStringProperty.
      If you want old value behavior, you must specify "<composed>" as wxStringProperty's
      value.

    Sample:

    @code
        wxPGId pid = pg->Append( new wxStringProperty(wxT("Car"),wxPG_LABEL,wxT("<composed>")) );

        pg->AppendIn( pid, new wxStringProperty(wxT("Model"),
                                                wxPG_LABEL,
                                                wxT("Lamborghini Diablo SV")) );

        pg->AppendIn( pid, new wxIntProperty(wxT("Engine Size (cc)"),
                                             wxPG_LABEL,
                                             5707) );

        wxPGId speedId = pg->AppendIn( pid, new wxStringProperty(wxT("Speeds"),wxPG_LABEL,wxT("<composed>")) );
        pg->AppendIn( speedId, new wxIntProperty(wxT("Max. Speed (mph)"),wxPG_LABEL,290) );
        pg->AppendIn( speedId, new wxFloatProperty(wxT("0-100 mph (sec)"),wxPG_LABEL,3.9) );
        pg->AppendIn( speedId, new wxFloatProperty(wxT("1/4 mile (sec)"),wxPG_LABEL,8.6) );

        // Make sure the child properties can be accessed correctly
        pg->SetPropertyValue( wxT("Car.Speeds.Max. Speed (mph)"), 300 );

        pg->AppendIn( pid, new wxIntProperty(wxT("Price ($)"),
                                             wxPG_LABEL,
                                             300000) );
        // Displayed value of "Car" property is now:
        // "Lamborghini Diablo SV; [300; 3.9; 8.6]; 300000"

    @endcode

    @section enumandflags wxEnumProperty and wxFlagsProperty

      wxEnumProperty is used when you want property's (integer or string) value
    to be selected from a popup list of choices.

      Creating wxEnumProperty is more complex than those described earlier.
    You have to provide list of constant labels, and optionally relevant values
    (if label indexes are not sufficient).

    @remarks

    - Value wxPG_INVALID_VALUE (equals 2147483647 which usually equals INT_MAX) is not
      allowed as value.

    A very simple example:

    @code

        //
        // Using wxArrayString
        //
        wxArrayString arrDiet;
        arr.Add(wxT("Herbivore"));
        arr.Add(wxT("Carnivore"));
        arr.Add(wxT("Omnivore"));

        pg->Append( new wxEnumProperty(wxT("Diet"),
                                       wxPG_LABEL,
                                       arrDiet) );



        //
        // Using wxChar* array
        //
        const wxChar* arrayDiet[] =
        { wxT("Herbivore"), wxT("Carnivore"), wxT("Omnivore"), NULL };

        pg->Append( new wxEnumProperty(wxT("Diet"),
                                       wxPG_LABEL,
                                       arrayDiet) );


    @endcode

    Here's extended example using values as well:

    @code

        //
        // Using wxArrayString and wxArrayInt
        //
        wxArrayString arrDiet;
        arr.Add(wxT("Herbivore"));
        arr.Add(wxT("Carnivore"));
        arr.Add(wxT("Omnivore"));

        wxArrayInt arrIds;
        arrIds.Add(40);
        arrIds.Add(45);
        arrIds.Add(50);

        // Note that the initial value (the last argument) is the actual value,
        // not index or anything like that. Thus, our value selects "Omnivore".
        pg->Append( new wxEnumProperty(wxT("Diet"),
                                       wxPG_LABEL,
                                       arrDiet,
                                       arrIds,
                                       50));


        //
        // Using wxChar* and long arrays
        //
        const wxChar* array_diet[] =
        { wxT("Herbivore"), wxT("Carnivore"), wxT("Omnivore"), NULL };

        long array_diet_ids[] =
        { 40, 45, 50 };

        // Value can be set from string as well
        pg->Append( new wxEnumProperty(wxT("Diet"),
                                       wxPG_LABEL,
                                       array_diet,
                                       array_diet_ids);

    @endcode

      wxPGChoices is a class where wxEnumProperty, and other properties which
      require label storage, actually stores strings and values. It is used
      to facilitiate reference counting, and therefore recommended way of
      adding items when multiple properties share the same set.

      You can use wxPGChoices directly as well, filling it and then passing it
      to the constructor. Infact, if you wish to display bitmaps next to labels,
      your best choice is to use this approach.

    @code

        wxPGChoices chs;
        chs.Add(wxT("Herbivore"),40);
        chs.Add(wxT("Carnivore"),45);
        chs.Add(wxT("Omnivore"),50);

        // Let's add an item with bitmap, too
        chs.Add(wxT("None of the above"), wxBitmap(), 60);

        // Note: you can add even whole arrays to wxPGChoices

        pg->Append( new wxEnumProperty(wxT("Diet"),
                                       wxPG_LABEL,
                                       chs) );

        // Add same choices to another property as well - this is efficient due
        // to reference counting
        pg->Append( new wxEnumProperty(wxT("Diet 2"),
                                       wxPG_LABEL,
                                       chs) );

     @endcode

    If you later need to change choices used by a property, there is function
    for that as well.

    @code

        //
        // Example 1: Add one extra item
        wxPGChoices& choices = pg->GetPropertyChoices(wxT("Diet"));
        choices.Add(wxT("Custom"),55);

        //
        // Example 2: Replace all the choices
        wxPGChoices chs;
        chs.Add(wxT("<No valid items yet>"),0);
        pg->SetPropertyChoices(wxT("Diet"),chs);

    @endcode

    If you want to create your enum properties with simple (label,name,value)
    constructor, then you need to create a new property class using one of the
    supplied macro pairs. See @ref newprops for details.

    <b>wxEditEnumProperty</b> is works exactly like wxEnumProperty, except
    is uses non-readonly combobox as default editor, and value is stored as
    string when it is not any of the choices.

    wxFlagsProperty is similar:

    @code

        const wxChar* flags_prop_labels[] = { wxT("wxICONIZE"),
            wxT("wxCAPTION"), wxT("wxMINIMIZE_BOX"), wxT("wxMAXIMIZE_BOX"), NULL };

        // this value array would be optional if values matched string indexes
        long flags_prop_values[] = { wxICONIZE, wxCAPTION, wxMINIMIZE_BOX,
            wxMAXIMIZE_BOX };

        pg->Append( new wxFlagsProperty(wxT("Window Style"),
                                        wxPG_LABEL,
                                        flags_prop_labels,
                                        flags_prop_values,
                                        wxDEFAULT_FRAME_STYLE) );

    @endcode

    wxFlagsProperty can use wxPGChoices just the same way as wxEnumProperty
    (and also custom property classes can be created with similar macro pairs).
    <b>Note: </b> When changing "choices" (ie. flag labels) of wxFlagsProperty,
    you will need to use SetPropertyChoices - otherwise they will not get updated
    properly.

    @section advprops Specialized Properties

      This section describes the use of less often needed property classes.
    To use them, you have to include <wx/propgrid/advprops.h>.

    @code

    // Necessary extra header file
    #include <wx/propgrid/advprops.h>

    ...

        // Date property.
        pg->Append( new wxDateProperty(wxT("MyDateProperty"),
                                       wxPG_LABEL,
                                       wxDateTime::Now()) );

        // Image file property. Wildcard is auto-generated from available
        // image handlers, so it is not set this time.
        pg->Append( new wxImageFileProperty(wxT("Label of ImageFileProperty"),
                                            wxT("NameOfImageFileProp")) );

        // Font property has sub-properties. Note that we give window's font as
        // initial value.
        pg->Append( new wxFontProperty(wxT("Font"),
                                       wxPG_LABEL,
                                       GetFont()) );

        // Colour property with arbitrary colour.
        pg->Append( new wxColourProperty(wxT("My Colour 1"),
                                         wxPG_LABEL,
                                         wxColour(242,109,0) ) );

        // System colour property.
        pg->Append( new wxSystemColourProperty(wxT("My SysColour 1"),
                                               wxPG_LABEL,
                                               wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW)) );

        // System colour property with custom colour.
        pg->Append( new wxSystemColourProperty(wxT("My SysColour 2"),
                                               wxPG_LABEL,
                                               wxColour(0,200,160) ) );

        // Cursor property
        pg->Append( new wxCursorProperty(wxT("My Cursor"),
                                         wxPG_LABEL,
                                         wxCURSOR_ARROW));

    @endcode


    @section iterating Iterating through a property container

    You can use somewhat STL'ish iterator classes to iterate through the grid.
    Here is a simple example of forward iterating through all individual
    properties (not categories or sub-propeties that are normally 'transparent'
    to application code):

    @code

        wxPropertyGridIterator it;

        for ( it = pg->GetIterator();
              !it.AtEnd();
              it++ )
        {
            wxPGProperty* p = *it;
            // Do something with the property
        }

	@endcode

    As expected there is also a const iterator:

    @code

        wxPropertyGridConstIterator it;

        for ( it = pg->GetIterator();
              !it.AtEnd();
              it++ )
        {
            const wxPGProperty* p = *it;
            // Do something with the property
        }

	@endcode

    You can give some arguments to GetIterator to determine which properties
    get automatically filtered out. For complete list of options, see
    @link iteratorflags List of Property Iterator Flags@endlink. GetIterator()
    also accepts other arguments. See wxPropertyGridInterface::GetIterator()
    for details.
    
    This example reverse-iterates through all visible items:

    @code

        wxPropertyGridIterator it;

        for ( it = pg->GetIterator(wxPG_ITERATE_VISIBLE, wxBOTTOM);
              !it.AtEnd();
              it-- )
        {
            wxPGProperty* p = *it;
            // Do something with the property
        }

	@endcode

    <b>wxPython Note:</b> Instead of ++ operator, use Next() method, and instead of
    * operator, use GetProperty() method.

    GetIterator() only works with wxPropertyGrid and the individual pages
    of wxPropertyGridManager. In order to iterate through an arbitrary
    property container, you need to use wxPropertyGridInterface::GetVIterator().
    Note however that this virtual iterater is limited to forward iteration.

    @code

        wxPGVIterator it;

        for ( it = manager->GetVIterator();
              !it.AtEnd();
              it.Next() )
        {
            wxPGProperty* p = it.GetProperty();
            // Do something with the property
        }

	@endcode


    @section operations More About Operating with Properties

    Getting value of selected wxSystemColourProperty (which value type is derived
    from wxObject):

    @code

        wxPGId id = pg->GetSelection();

        if ( id )
        {
            // Get name of property
            const wxString& name = pg->GetPropertyName( id );

            // If type is not correct, GetColour() method will produce run-time error
            if ( pg->GetPropertyValueType() == wxT("wxColourPropertyValue") ) )
            {
                wxColourPropertyValue* pcolval =
                    wxDynamicCast(pg->GetPropertyValueAsWxObjectPtr(id),
                                  wxColourPropertyValue);

                // Report value
                wxString text;
                if ( pcolval->m_type == wxPG_CUSTOM_COLOUR )
                    text.Printf( wxT("It is custom colour: (%i,%i,%i)"),
                        (int)pcolval->m_colour.Red(),
                        (int)pcolval->m_colour.Green(),
                        (int)pcolval->m_colour.Blue());
                else
                    text.Printf( wxT("It is wx system colour (number=%i): (%i,%i,%i)"),
                        (int)pcolval->m_type,
                        (int)pcolval->m_colour.Red(),
                        (int)pcolval->m_colour.Green(),
                        (int)pcolval->m_colour.Blue());

                wxMessageBox( text );
            }
        }

    @endcode

    @section populating Populating wxPropertyGrid Automatically

    @subsection fromvariants Populating from List of wxVariants

    Example of populating an empty wxPropertyGrid from a values stored
    in an arbitrary list of wxVariants.

    @code

        // This is a static method that initializes *all* builtin type handlers
        // available, including those for wxColour and wxFont. Refers to *all*
        // included properties, so when compiling with static library, this
        // method may increase the executable size significantly.
        pg->InitAllTypeHandlers();

        // Get contents of the grid as a wxVariant list
        wxVariant all_values = pg->GetPropertyValues();

        // Populate the list with values. If a property with appropriate
        // name is not found, it is created according to the type of variant.
        pg->SetPropertyValues( my_list_variant );

        // In order to get wxObject ptr from a variant value,
        // wxGetVariantCast(VARIANT,CLASSNAME) macro has to be called.
        // Like this:
        wxVariant v_txcol = pg->GetPropertyValue(wxT("Text Colour"));
        const wxColour& txcol = wxGetVariantCast(v_txcol,wxColour);

	@endcode

    @subsection fromfile Loading Population from a Text-based Storage

    Class wxPropertyGridPopulator may be helpful when writing code that
    loads properties from a text-source. In fact, the supplied xrc handler
    (src/xh_propgrid.cpp) uses it. See that code for more info.
    NOTE: src/xh_propgrid.cpp is not included in the library by default,
    to avoid dependency to wxXRC. You will need to add it to your application
    separately.

    @subsection editablestate Saving and Restoring User-Editable State

    You can use wxPropertyGridInterface::SaveEditableState() and
    wxPropertyGridInterface::RestoreEditableState() to save and restore
    user-editable state (selected property, expanded/collapsed properties,
    selected page, scrolled position, and splitter positions).

    @section events Event Handling

    Probably the most important event is the Changed event which occurs when
    value of any property is changed by the user. Use EVT_PG_CHANGED(id,func)
    in your event table to use it.

    For complete list of event types, see wxPropertyGrid class reference.

    The custom event class, wxPropertyGridEvent, has methods to directly
    access the property that triggered the event.

    Here's a small sample:

    @code

    // Portion of an imaginary event table
    BEGIN_EVENT_TABLE(MyForm, wxFrame)

        ...

        // This occurs when a property value changes
        EVT_PG_CHANGED( PGID, MyForm::OnPropertyGridChange )

        ...

    END_EVENT_TABLE()

    void MyForm::OnPropertyGridChange( wxPropertyGridEvent& event )
    {
        wxPGProperty *property = event.GetProperty();

        // It may be NULL
        if ( !property )
            return;

        // Get name of changed property
        const wxString& name = property->GetName();

        // Get resulting value
        wxVariant value = property->GetValue();
    }

    @endcode

    Another event type you might find useful is EVT_PG_CHANGING, which occurs
    just prior property value is being changed by user. You can acquire pending
    value using wxPropertyGridEvent::GetValue(), and if it is not acceptable,
    call wxPropertyGridEvent::Veto() to prevent the value change from taking
    place.

    @code

    // Portion of an imaginary event table
    BEGIN_EVENT_TABLE(MyForm, wxFrame)

        ...

        // This occurs when a property value changes
        EVT_PG_CHANGING( PGID, MyForm::OnPropertyGridChanging )

        ...

    END_EVENT_TABLE()

    void MyForm::OnPropertyGridChanging( wxPropertyGridEvent& event )
    {
        wxPGProperty* property = event.GetProperty();

        if ( property == m_pWatchThisProperty )
        {
            // GetValue() returns the pending value, but is only
            // supported by wxEVT_PG_CHANGING.
            if ( event.GetValue().GetString() == g_pThisTextIsNotAllowed )
            {
                event.Veto();
                return;
            }
        }
    }

    @endcode

    @remarks On Sub-property Event Handling
    - For aggregate type properties (wxFontProperty, wxFlagsProperty, etc), events
      occur for the main parent property only. For other properties events occur
      for the children themselves..

    - When property's child gets changed, you can use wxPropertyGridEvent::GetMainParent
      to obtain its topmost non-category parent (useful, if you have deeply nested
      properties).


    @section validating Validating Property Values

    There are various ways to make sure user enters only correct values. First, you
    can use wxValidators similar to as you would with ordinary controls. Use
    wxPropertyGridInterface::SetPropertyValidator() to assign wxValidator to 
    property.

    Second, you can subclass a property and override wxPGProperty::ValidateValue(),
    or handle wxEVT_PG_CHANGING for the same effect. Both of these methods do not
    actually prevent user from temporarily entering invalid text, but they do give
    you an opportunity to warn the user and block changed value from being committed
    in a property.

    Various validation failure options can be controlled globally with
    wxPropertyGrid::SetValidationFailureBehavior(), or on an event basis by
    calling wxEvent::SetValidationFailureBehavior(). Here's a code snippet of
    how to handle wxEVT_PG_CHANGING, and to set custom failure behaviour and
    message.

    @code
        void MyFrame::OnPropertyGridChanging(wxPropertyGridEvent& event)
        {
            wxPGProperty* property = event.GetProperty();

            // You must use wxPropertyGridEvent::GetValue() to access
            // the value to be validated.
            wxVariant pendingValue = event.GetValue();

            if ( property->GetName() == wxT("Font") )
            {
                // Make sure value is not unspecified
                if ( !pendingValue.IsNull() )
                {
                    wxFont font << pendingValue;

                    // Let's just allow Arial font
                    if ( font.GetFaceName() != wxT("Arial") )
                    {
                        event.Veto();
                        event.SetValidationFailureBehavior(wxPG_VFB_STAY_IN_PROPERTY |
                                                           wxPG_VFB_BEEP |
                                                           wxPG_VFB_SHOW_MESSAGE);
                    }
                }
            }
        }
    @endcode


    @section cellrender Customizing Individual Cell Appearance

    You can control text colour, background colour, and attached image of
    each cell in the property grid. Use wxPropertyGridInterface::SetPropertyCell() or
    wxPGProperty::SetCell() for this purpose.

    In addition, it is possible to control these characteristics for
    wxPGChoices list items. See wxPGChoices::Item() and wxPGChoiceEntry class
    reference for more info.


    @section customizing Customizing Properties (without sub-classing)

    In this section are presented miscellaneous ways to have custom appearance
    and behavior for your properties without all the necessary hassle
    of sub-classing a property class etc.

    @subsection customimage Setting Value Image

    Every property can have a small value image placed in front of the
    actual value text. Built-in example of this can be seen with
    wxColourProperty and wxImageFileProperty, but for others it can
    be set using wxPropertyGrid::SetPropertyImage method.

    @subsection customvalidator Setting Validator

    You can set wxValidator for a property using wxPropertyGrid::SetPropertyValidator.

    Validator will work just like in wxWidgets (ie. editorControl->SetValidator(validator)
    is called).

    @subsection customeditor Setting Property's Editor Control(s)

    You can set editor control (or controls, in case of a control and button),
    of any property using wxPropertyGrid::SetPropertyEditor. Editors are passed
    using wxPG_EDITOR(EditorName) macro, and valid built-in EditorNames are
    TextCtrl, Choice, ComboBox, CheckBox, TextCtrlAndButton, ChoiceAndButton,
    SpinCtrl, and DatePickerCtrl. Two last mentioned ones require call to
    static member function wxPropertyGrid::RegisterAdditionalEditors().

    Following example changes wxColourProperty's editor from default Choice
    to TextCtrlAndButton. wxColourProperty has its internal event handling set
    up so that button click events of the button will be used to trigger
    colour selection dialog.

    @code

        wxPGId colProp = pg->Append(wxColourProperty(wxT("Text Colour")));

        pg->SetPropertyEditor(colProp,wxPG_EDITOR(TextCtrlAndButton));

    @endcode

    Naturally, creating and setting custom editor classes is a possibility as
    well. For more information, see wxPGEditor class reference.

    @subsection editorattrs Property Attributes Recognized by Editors

    <b>SpinCtrl</b> editor can make use of property's "Min", "Max", "Step",
    "Wrap" and "MotionSpin" attributes.

    @subsection multiplebuttons Adding Multiple Buttons Next to an Editor

    See wxPGMultiButton class reference.

    @subsection customeventhandling Handling Events Passed from Properties

    <b>wxEVT_COMMAND_BUTTON_CLICKED </b>(corresponds to event table macro EVT_BUTTON):
    Occurs when editor button click is not handled by the property itself
    (as is the case, for example, if you set property's editor to TextCtrlAndButton
    from the original TextCtrl).

    @subsection attributes Property Attributes

    Miscellaneous values, often specific to a property type, can be set
    using wxPropertyGrid::SetPropertyAttribute and wxPropertyGrid::SetPropertyAttributeAll
    methods.

    Attribute names are strings and values wxVariant. Arbitrary names are allowed
    inorder to store user values. Constant equivalents of all attribute string names are
    provided. Some of them are defined as cached strings, so using constants can provide
    for smaller binary size.

    For complete list of attributes, see @link attrids Property Attributes@endlink.

    @subsection boolcheckbox Setting wxBoolProperties to Use Check Box

    To have all wxBoolProperties to use CheckBox editor instead of Choice, use
    following (call after bool properties have been added):

    @code
        pg->SetPropertyAttributeAll(wxPG_BOOL_USE_CHECKBOX,true);
    @endcode


    @section usage2 Using wxPropertyGridManager

    wxPropertyGridManager is an efficient multi-page version of wxPropertyGrid,
    which can optionally have toolbar for mode and page selection, and a help text
    box.

    wxPropertyGridManager inherits from wxPropertyGridInterface, and as such
    it has most property manipulation functions. However, only some of them affect
    properties on all pages (eg. GetPropertyByName() and ExpandAll()), while some
    (eg. Append()) only apply to the currently selected page.

    To operate explicitly on properties on specific page, use wxPropertyGridManager::GetPage()
    to obtain pointer to page's wxPropertyGridPage object.

    Visual methods, such as SetCellBackgroundColour and GetNextVisible are only
    available in wxPropertyGrid. Use wxPropertyGridManager::GetGrid() to obtain
    pointer to it.

    Iteration methods will not work in wxPropertyGridManager. Instead, you must acquire
    the internal grid (GetGrid()) or wxPropertyGridPage object (GetPage()).

    wxPropertyGridManager constructor has exact same format as wxPropertyGrid
    constructor, and basicly accepts same extra window style flags (albeit also
    has some extra ones).

    Here's some example code for creating and populating a wxPropertyGridManager:

    @code

        wxPropertyGridManager* pgMan = new wxPropertyGridManager(this, PGID,
            wxDefaultPosition, wxDefaultSize,
            // These and other similar styles are automatically
            // passed to the embedded wxPropertyGrid.
            wxPG_BOLD_MODIFIED|wxPG_SPLITTER_AUTO_CENTER|
            // Include toolbar.
            wxPG_TOOLBAR |
            // Include description box.
            wxPG_DESCRIPTION |
            // Include compactor.
            wxPG_COMPACTOR |
            // Plus defaults.
            wxPGMAN_DEFAULT_STYLE
           );

        wxPropertyGridPage* page;

        pgMan->AddPage(wxT("First Page"));
        page = pgMan->GetLastPage();

        page->Append( new wxPropertyCategory(wxT("Category A1")) );

        page->Append( new wxIntProperty(wxT("Number"),wxPG_LABEL,1) );

        page->Append( new wxColourProperty(wxT("Colour"),wxPG_LABEL,*wxWHITE) );

        pgMan->AddPage(wxT("Second Page"));
        page = pgMan->GetLastPage();

        page->Append( wxT("Text"),wxPG_LABEL,wxT("(no text)") );

        page->Append( new wxFontProperty(wxT("Font"),wxPG_LABEL) );

    @endcode

    @subsection propgridpage wxPropertyGridPage

    wxPropertyGridPage is holder of properties for one page in manager. It is derived from
    wxEvtHandler, so you can subclass it to process page-specific property grid events. Hand
    over your page instance in wxPropertyGridManager::AddPage.

    Please note that the wxPropertyGridPage itself only sports subset of wxPropertyGrid API
    (but unlike manager, this include item iteration). Naturally it inherits from
    wxPropertyGridMethods and wxPropertyGridState.


    @section subclassing Subclassing wxPropertyGrid and wxPropertyGridManager

    Few things to note:

    - Only a small percentage of member functions are virtual. If you need more,
      just let me know.

    - Data manipulation is done in wxPropertyGridState class. So, instead of
      overriding wxPropertyGrid::Insert, you'll probably want to override wxPropertyGridState::DoInsert.

    - Override wxPropertyGrid::CreateState to instantiate your derivate wxPropertyGridState.
      For wxPropertyGridManager, you'll need to subclass wxPropertyGridPage instead (since it
      is derived from wxPropertyGridState), and hand over instances in wxPropertyGridManager::AddPage
      calls.

    - You can use a derivate wxPropertyGrid with manager by overriding wxPropertyGridManager::CreatePropertyGrid
      member function.


    @section misc Miscellaneous Topics

    @subsection namescope Property Name Scope

      All properties which parent is category or root can be accessed
    directly by their base name (ie. name given for property in its constructor).
    Other properties can be accessed via "ParentsName.BaseName" notation,
    Naturally, all property names should be unique.

    @subsection nonuniquelabels Non-unique Labels

      It is possible to have properties with identical label under same parent.
    However, care must be taken to ensure that each property still has
    unique (base) name.

    @subsection boolproperty wxBoolProperty

      There are few points about wxBoolProperty that require futher discussion:
      - wxBoolProperty can be shown as either normal combobox or as a checkbox.
        Property attribute wxPG_BOOL_USE_CHECKBOX is used to change this.
        For example, if you have a wxFlagsProperty, you can
        set its all items to use check box using the following:
        @code
            pg->SetPropertyAttribute(wxT("MyFlagsProperty"),wxPG_BOOL_USE_CHECKBOX,true,wxPG_RECURSE);
        @endcode

      - Default item names for wxBoolProperty are [wxT("False"),wxT("True")]. This can be
        changed using wxPropertyGrid::SetBoolChoices(trueChoice,falseChoice).

    @subsection textctrlupdates Updates from wxTextCtrl Based Editor

      Changes from wxTextCtrl based property editors are committed (ie.
    wxEVT_PG_CHANGED is sent etc.) *only* when (1) user presser enter, (2)
    user moves to edit another property, or (3) when focus leaves
    the grid.

      Because of this, you may find it useful, in some apps, to call
    wxPropertyGrid::CommitChangesFromEditor() just before you need to do any
    computations based on property grid values. Note that CommitChangesFromEditor()
    will dispatch wxEVT_PG_CHANGED with ProcessEvent, so any of your event handlers
    will be called immediately.

    @subsection splittercentering Centering the Splitter

      If you need to center the splitter, but only once when the program starts,
    then do <b>not</b> use the wxPG_SPLITTER_AUTO_CENTER window style, but the
    wxPropertyGrid::CenterSplitter() method. <b>However, be sure to call it after
    the sizer setup and SetSize calls!</b> (ie. usually at the end of the
    frame/dialog constructor)

    @subsection splittersetting Setting Splitter Position When Creating Property Grid

    Splitter position cannot exceed grid size, and therefore setting it during
    form creation may fail as initial grid size is often smaller than desired
    splitter position, especially when sizers are being used.

    @subsection colourproperty wxColourProperty and wxSystemColourProperty

    Through subclassing, these two property classes provide substantial customization
    features. Subclass wxSystemColourProperty if you want to use wxColourPropertyValue
    (which features colour type in addition to wxColour), and wxColourProperty if plain
    wxColour is enough.
  
    Override wxSystemColourProperty::ColourToString() to redefine how colours are
    printed as strings.

    Override wxSystemColourProperty::GetCustomColourIndex() to redefine location of
    the item that triggers colour picker dialog (default is last).

    Override wxSystemColourProperty::GetColour() to determine which colour matches
    which choice entry.


    @subsection compilerdefines Supported Preprocessor Defines

    Here is list of supported preprocessor defines (other than those that relate with
    wxWidgets core library):

    <b>wxPG_INCLUDE_MANAGER:</b> Defines as 0 to exclude wxPropertyGridManager from compilation.

    <b>wxPG_INCLUDE_ADVPROPS:</b> Defines as 0 to exclude properties and editors defined in advprops.h
      and advprops.cpp.

    <b>wxPG_USE_WXMODULE:</b> Define as 0 to not use wxModule to manage global variables.
    This may be needed in cases where wxPropertyGrid is linked as a plugin DLL, or when
    wxPropertyGrid is linked statically in a DLL.

    <b>WXMAKINGLIB_PROPGRID:</b> Define if you are linking wxPropertyGrid statically
    but wxWidgets itself is DLL.

    <b>WXMAKINGDLL_PROPGRID:</b> Define when building wxPropertyGrid as a DLL. This
    should be automatically defined correctly by the Bakefile-generated makefiles.

    <b>wxPG_USE_STL:</b> Define as 1 to let wxPropertyGrid use STL classes in API
    as much as possible. Default value equals wxUSE_STL.

    <b>wxPG_COMPATIBILITY_1_2_0:</b> Define as 1 to make wxPropertyGrid more compatible with the
    old 1.2.x releases.

    <b>wxPG_COMPATIBILITY_1_0_0:</b> Define to make wxPropertyGrid more compatible with the
    old 1.0.x releases.


    @section proplist Property Class Descriptions

    Here are descriptions of built-in properties, with attributes
    (see wxPropertyGrid::SetPropertyAttribute) that apply to them.
    Note that not all attributes are necessarily here. For complete
    list, see @link attrids Property Attributes@endlink.

    @subsection wxPropertyCategory

    Not an actual property per se, but a header for a group of properties.

    @subsection wxStringProperty

    <b>Inheritable Class:</b> wxStringProperty

    Simple string property. wxPG_STRING_PASSWORD attribute may be used
    to echo value as asterisks and use wxTE_PASSWORD for wxTextCtrl.

    @remarks
    * wxStringProperty has a special trait: if it has value of "<composed>",
      and also has child properties, then its displayed value becomes
      composition of child property values, similar as with wxFontProperty,
      for instance.

    @subsection wxIntProperty

    Like wxStringProperty, but converts text to a signed long integer.
    wxIntProperty seemlessly supports 64-bit integers (ie. wxLongLong).

    @subsection wxUIntProperty

    Like wxIntProperty, but displays value as unsigned int. To set
    the prefix used globally, manipulate wxPG_UINT_PREFIX string attribute.
    To set the globally used base, manipulate wxPG_UINT_BASE int
    attribute. Regardless of current prefix, understands (hex) values starting
    with both "0x" and "$".
    wxUIntProperty seemlessly supports 64-bit unsigned integers (ie. wxULongLong).

    @subsection wxFloatProperty

    Like wxStringProperty, but converts text to a double-precision floating point.
    Default float-to-text precision is 6 decimals, but this can be changed
    by modifying wxPG_FLOAT_PRECISION attribute.

    @subsection wxBoolProperty

    Represents a boolean value. wxChoice is used as editor control, by the
    default. wxPG_BOOL_USE_CHECKBOX attribute can be set to true inorder to use
    check box instead.

    @subsection wxLongStringProperty

    Like wxStringProperty, but has a button that triggers a small text editor
    dialog.

    @subsection wxDirProperty

    Like wxLongStringProperty, but the button triggers dir selector instead.

    @subsection wxFileProperty

    Like wxLongStringProperty, but the button triggers file selector instead.
    Default wildcard is "All files..." but this can be changed by setting
    wxPG_FILE_WILDCARD attribute (see wxFileDialog for format details).
    Attribute wxPG_FILE_SHOW_FULL_PATH can be set to false inorder to show
    only the filename, not the entire path.

    @subsection wxEnumProperty

    Represents a single selection from a list of choices -
    custom combobox control is used to edit the value.

    @subsection wxFlagsProperty

    Represents a bit set that fits in a long integer. wxBoolProperty sub-properties
    are created for editing individual bits. Textctrl is created to manually edit
    the flags as a text; a continous sequence of spaces, commas and semicolons
    is considered as a flag id separator.
    <b>Note: </b> When changing "choices" (ie. flag labels) of wxFlagsProperty, you
    will need to use SetPropertyChoices - otherwise they will not get updated properly.

    @subsection wxArrayStringProperty

    Allows editing of a list of strings in wxTextCtrl and in a separate dialog.

    @subsection wxDateProperty

    wxDateTime property. Default editor is DatePickerCtrl, altough TextCtrl
    should work as well. wxPG_DATE_FORMAT attribute can be used to change
    string wxDateTime::Format uses (altough default is recommended as it is
    locale-dependant), and wxPG_DATE_PICKER_STYLE allows changing window
    style given to DatePickerCtrl (default is wxDP_DEFAULT|wxDP_SHOWCENTURY).

    @subsection wxEditEnumProperty

    Represents a string that can be freely edited or selected from list of choices -
    custom combobox control is used to edit the value.

    @subsection wxMultiChoiceProperty

    Allows editing a multiple selection from a list of strings. This is
    property is pretty much built around concept of wxMultiChoiceDialog.
    It uses wxArrayString value.

    @subsection wxImageFileProperty

    Like wxFileProperty, but has thumbnail of the image in front of
    the filename and autogenerates wildcard from available image handlers.

    @subsection wxColourProperty

    <b>Useful alternate editor:</b> Choice.

    Represents wxColour. wxButton is used to trigger a colour picker dialog.

    @subsection wxFontProperty

    Represents wxFont. Various sub-properties are used to edit individual
    subvalues.

    @subsection wxSystemColourProperty

    Represents wxColour and a system colour index. wxChoice is used to edit
    the value. Drop-down list has color images.

    @subsection wxCursorProperty

    Represents a wxCursor. wxChoice is used to edit the value.
    Drop-down list has cursor images under some (wxMSW) platforms.

    @subsection Additional Sample Properties

    Sample application has following additional examples of custom properties:
    - wxFontDataProperty ( edits wxFontData )
    - wxPointProperty ( edits wxPoint )
    - wxSizeProperty ( edits wxSize )
    - wxAdvImageFileProperty ( like wxImageFileProperty, but also has a drop-down for recent image selection)
    - wxDirsProperty ( edits a wxArrayString consisting of directory strings)
    - wxArrayDoubleProperty ( edits wxArrayDouble )

    @section userhelp Using wxPropertyGrid control

    This is a short summary of how a wxPropertyGrid is used (not how it is programmed),
    or, rather, how it <b>should</b> behave in practice.

    - Basic mouse usage is as follows:\n
      - Clicking property label selects it.
      - Clicking property value selects it and focuses to editor control.
      - Clicking category label selects the category.
      - Double-clicking category label selects the category and expands/collapses it.
      - Double-clicking labels of a property with children expands/collapses it.

    - Keyboard usage is as follows:\n
      - alt + down (or right) - displays editor dialog (if any) for a property. Note
        that this shortcut can be changed using wxPropertyGrid::SetButtonShortcut.\n
      Only when editor control is not focused:\n
      - cursor up - moves to previous visible property\n
      - cursor down - moves to next visible property\n
      - cursor left - if collapsible, collapses, otherwise moves to previous property\n
      - cursor right - if expandable, expands, otherwise moves to next property\n
      - tab (if enabled) - focuses keyboard to the editor control of selected property
        NOTE: This doesn't work at the moment (and is unlikely to be fixed). Same applies
              to other tab key combos described below.\n
      Only when editor control is focused:\n
      - return/enter - confirms changes made to a wxTextCtrl based editor\n
      - tab - moves to next visible property (or, if in last one, moves out of grid)\n
      - shift-tab - moves to previous visible property (or, if in first one, moves out of grid)\n
      - escape - unfocuses from editor control and cancels any changes made
        (latter for wxTextCtrl based editor only)\n

    - In long strings tabs are represented by "\t" and line break by "\n".

    @section notes Design Notes
    - Currently wxPropertyGridManager uses "easy" way to relay events from embedded
      wxPropertyGrid. That is, the exact same id is used for both.

    - If wxPG_DOUBLE_BUFFER is 1 (default for MSW, GTK and MAC), wxPropertyGrid::DoDrawItems
      composes the image on a wxMemoryDC. This effectively eliminates flicker caused by drawing
      itself (property editor controls are another matter).

    - Under wxMSW, flicker freedom when creating native editor controls is achieved by using
      following undocumented scheme:
      @code
        wxControl* ctrl = new wxControl();
      #ifdef __WXMSW__
        ctrl->Hide();
      #endif
        ctrl->Create(parent,id,...);

        ...further initialize, move, resize, etc...

      #ifdef __WXMSW__
        ctrl->Show();
      #endif
      @endcode

    @section crossplatform Crossplatform Notes (not necessarily wxPropertyGrid specific)

    - GTK1: When showing a dialog you may encounter invisible font!
      Solution: Set parent's font using SetOwnFont instead of SetFont.

    - GTK: Your own control can overdraw wxGTK wxWindow border!

    - wxWindow::SetSizeHints may be necessary to shrink controls below certain treshold,
      but only on some platforms. For example wxMSW might allow any shrinking without
      SetSizeHints call where wxGTK might not.

    - GTK Choice (atleast, maybe other controls as well) likes its items set
      in constructor. Appending them seems to be slower (Freeze+Thaw won't help).
      Even using Append that gets wxArrayString argument may not be good, since it
      may just append every string one at a time.


    @section newprops Creating New Properties

    Easiest solution for creating an arbitrary property is to subclass an existing,
    property class that most resembles the intended end result. You may need to include
    header file wx/propgrid/propdev.h.

    For instance:

    @code

    #include <wx/propgrid/propdev.h>

    //
    // wxLongStringProperty has wxString as value type and TextCtrlAndButton as editor.
    // Here we will derive a new property class that will show single choice dialog
    // on button click.
    //

    class MyStringProperty : public wxLongStringProperty
    {
        DECLARE_DYNAMIC_CLASS(MyStringProperty)
    public:

        // Normal property constructor.
        MyStringProperty(const wxString& label,
                         const wxString& name = wxPG_LABEL,
                         const wxString& value = wxEmptyString)
            : wxLongStringProperty(name,label,value)
        {
            // Prepare choices
            m_choices.Add(wxT("Cat"));
            m_choices.Add(wxT("Dog"));
            m_choices.Add(wxT("Gibbon"));
            m_choices.Add(wxT("Otter"));
        }

        // Do something special when button is clicked.
        virtual wxPGEditorDialogAdapter* GetEditorDialog() const
        {
            return new wxSingleChoiceDialogAdapter(m_choices);
        }

    protected:
        wxPGChoices m_choices;
    };

    IMPLEMENT_DYNAMIC_CLASS(MyStringProperty, wxLongStringProperty)

    //
    // Actually implement the editor dialog adapter
    // Naturally, in real code this would have to come
    // before wxPGProperty::GetEditorDialog()
    // implementation.
    //

    class wxSingleChoiceDialogAdapter : public wxPGEditorDialogAdapter
    {
    public:

        wxSingleChoiceDialogAdapter( const wxPGChoices& choices )
            : wxPGEditorDialogAdapter(), m_choices(choices)
        {
        }

        virtual bool DoShowDialog( wxPropertyGrid* WXUNUSED(propGrid),
                                   wxPGProperty* WXUNUSED(property) )
        {
            wxString s = ::wxGetSingleChoice(wxT("Message"),
                                             wxT("Caption"),
                                             m_choices.GetLabels());
            if ( s.length() )
            {
                SetValue(s);
                return true;
            }

            return false;
        }

    protected:
        const wxPGChoices&  m_choices;
    };

    @endcode

    You can then create a property instance, for instance:

    @code

        pg->Append( new MyStringProperty(name,label,value) );

    @endcode

    If you want to change editor used, use code like below (continues our sample above).

    Note that built-in editors include: TextCtrl, Choice, ComboBox, TextCtrlAndButton,
    ChoiceAndButton, CheckBox, SpinCtrl, and DatePickerCtrl.

    @code

        // In class body:
        virtual const wxPGEditor* DoGetEditorClass() const
        {
            return wxPG_EDITOR(TextCtrl);
        }

    @endcode

    You can change the 'value type' of a property by simply assigning different type
    of variant with SetValue. <b>It is mandatory to implement wxVariantData class
    for all data types used as property values.</b> Also, it is further recommended
    to derive your class from wxPGVariantData (see class documentation for more info).

    @remarks
    - For practical examples of arbitrary properties, please take a look
      at the sample properties in samples/sampleprops.cpp.
    - Read wxPGProperty class documentation to find out what each virtual member
      function should do.
    - Documentation below may be helpful (altough you'd probably do better
      by looking at the sample properties first).

    @subsection methoda Macro Pairs

    These are quick methods for creating customized properties.

    @subsubsection custstringprop String Property with Button

    This custom property will be exactly the same as wxLongStringProperty,
    except that you can specify a custom code to handle what happens
    when the button is pressed.

      In header:
        @code
        WX_PG_DECLARE_STRING_PROPERTY(PROPNAME)
        @endcode

        In source:
        @code

        // FLAGS can be wxPG_NO_ESCAPE if escape sequences shall not be expanded.
        WX_PG_IMPLEMENT_STRING_PROPERTY(PROPNAME, FLAGS)

        bool PROPNAME::OnButtonClick( wxPropertyGrid* propgrid, wxString& value )
        {
            //
            // TODO: Show dialog, read initial string from value. If changed,
            //   store new string to value and return TRUE.
            // NB: You must use wxPGProperty::SetValueInEvent().
            //
        }
        @endcode

    FLAGS is either wxPG_NO_ESCAPE (newlines and tabs are not translated to and
    from escape sequences) or wxPG_ESCAPE (newlines and tabs are transformed
    into C-string escapes).

    There is also WX_PG_IMPLEMENT_STRING_PROPERTY_WITH_VALIDATOR variant which
    also allows setting up a validator for the property. Like this:

        @code

        WX_PG_IMPLEMENT_STRING_PROPERTY_WITH_VALIDATOR(PROPNAME, FLAGS)

        bool PROPNAME::OnButtonClick( wxPropertyGrid* propgrid, wxString& value )
        {
            //
            // TODO: Show dialog, read initial string from value. If changed,
            //   store new string to value and return TRUE.
            // NB: You must use wxPGProperty::SetValueInEvent().
            //
        }

        wxValidator* PROPNAME::DoGetValidator() const
        {
            //
            // TODO: Return pointer to a new wxValidator instance. In most situations,
            //   code like this should work well:
            //
            //    WX_PG_DOGETVALIDATOR_ENTRY()
            //
            //    wxMyValidator* validator = new wxMyValidator(...);
            //
            //    ... prepare validator...
            //
            //    WX_PG_DOGETVALIDATOR_EXIT(validator)
            //
            //  Macros are used to maintain only one actual validator instance
            //  (ie. on a second call, function exits within the first macro).
            //
            //  For real examples, search props.cpp for ::DoGetValidator, it should
            //  have several.
            //
        }
        @endcode

    @subsubsection custflagsprop Custom Flags Property
    Flags property with custom default value and built-in labels/values.

    In header:
        @code
        WX_PG_DECLARE_CUSTOM_FLAGS_PROPERTY(PROPNAME)
        @endcode

    In source:
        @code

        // LABELS are VALUES are as in the arguments to wxFlagsProperty
        // constructor. DEFVAL is the new default value (normally it is 0).
        WX_PG_IMPLEMENT_CUSTOM_FLAGS_PROPERTY(PROPNAME,LABELS,VALUES,DEFAULT_FLAGS)
        @endcode

    The new property class will have simple (label,name,value) constructor.

    @subsubsection custenumprop Custom EnumProperty

    Exactly the same as custom FlagsProperty. Simply replace FLAGS with ENUM in
    macro names to create wxEnumProperty based class instead.

    @subsubsection custarraystringprop Custom ArrayString property

    This type of custom property allows selecting different string delimiter
    (default is '"' on both sides of the string - as in C code), and allows
    adding custom button into the editor dialog.

    In header:
        @code
        WX_PG_DECLARE_ARRAYSTRING_PROPERTY(wxMyArrayStringProperty)
        @endcode

    In source:

        @code

        // second argument = string delimiter. '"' for C string style (default),
        //    and anything else for str1<delimiter> str2<delimiter> str3 style
        //    (so for example, using ';' would result to str1; str2; str3).
        // third argument = const wxChar* text for the custom button. If NULL
        //    then no button is added.
        WX_PG_IMPLEMENT_ARRAYSTRING_PROPERTY(wxMyArrayStringProperty,',',wxT("Browse"))

        bool wxMyArrayStringProperty::OnCustomStringEdit(wxWindow* parent,
                                                         wxString& value)
        {
            //
            // TODO: Show custom editor dialog, read initial string from value.
            //   If changed, store new string to value and return TRUE.
            //
        }

        @endcode

    @subsubsection custcolprop Custom ColourProperty

    wxColourProperty/wxSystemColourProperty that can have custom list of colours
    in dropdown.

    Use version that doesn't have _USES_WXCOLOUR in macro names to have
    wxColourPropertyValue as value type instead of plain wxColour (in this case
    values array might also make sense).

    In header:
        @code
        #include <wx/propgrid/advprops.h>
        WX_PG_DECLARE_CUSTOM_COLOUR_PROPERTY_USES_WXCOLOUR(wxMyColourProperty)
        @endcode

    In source:

        @code

        // Colour labels. Last (before NULL, if any) must be Custom.
        static const wxChar* mycolprop_labels[] = {
            wxT("Black"),
            wxT("Blue"),
            wxT("Brown"),
            wxT("Custom"),
            (const wxChar*) NULL
        };

        // Relevant colour values as unsigned longs.
        static unsigned long mycolprop_colours[] = {
            wxPG_COLOUR(0,0,0),
            wxPG_COLOUR(0,0,255),
            wxPG_COLOUR(166,124,81),
            wxPG_COLOUR(0,0,0)
        };

        // Implement property class. Third argument is optional values array,
        // but in this example we are only interested in creating a shortcut
        // for user to access the colour values.
        WX_PG_IMPLEMENT_CUSTOM_COLOUR_PROPERTY_USES_WXCOLOUR(wxMyColourProperty,
                                                             mycolprop_labels,
                                                             (long*)NULL,
                                                             mycolprop_colours)
        @endcode


    @section neweditors Creating Custom Property Editor

    - See the sources of built-in editors in contrib/src/propgrid/propgrid.cpp
      (search for wxPGTextCtrlEditor).

    - For additional information, see wxPGEditor class reference

    @subsection wxpythoneditors In wxPython

    - See README-propgrid-wxPython.txt

*/

#endif // __WX_PG_DOX_MAINPAGE_H__

