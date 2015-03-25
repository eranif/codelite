{
open:
    function(title, content, buttons, hideCloseButton)
    {
        // hide the notification(s)
        notification.hide();

        var popup = document.getElementById(popupId);
        // remove the previous popup window
        if(popup) {
            // hide the popup
            $('#' + popupId).modal('hide');

            // remove the DOM element
            popup.parentNode.removeChild(popup);
        }

        // create the popup element with "backdrop"
        popup = createHTMLElement(
            'div', { id : popupId, 'class' : 'modal fade', 'data-backdrop' : 'static', 'data-keyboard' : 'false' });
        var innerPopupDialog = createHTMLElement('div', { 'class' : 'modal-dialog' });
        var modalContent = createHTMLElement('div', { 'class' : 'modal-content' });

        // build the popup content (header, content and footer) - only what needed
        if(title) modalContent.appendChild(getModalHeader(title));
        if(content) modalContent.appendChild(getModalContent(content));
        if(buttons || !hideCloseButton) modalContent.appendChild(getModalFooter(buttons, hideCloseButton));

        // wrap it
        innerPopupDialog.appendChild(modalContent);
        popup.appendChild(innerPopupDialog);

        // append to the body
        document.body.appendChild(popup);

        // display the popup
        if(window.$) {
            var $elem = $('#' + popupId);
            if($elem && $elem.modal) {
                $elem.modal('show');
                $elem.attr('tabindex', '0').get(0).focus();
            }
        }
    }
    , close : function() {
        // hide the notification(s)
        notification.hide();

        var $elem = $('#' + popupId);
        $elem.modal('hide');
    }
};
