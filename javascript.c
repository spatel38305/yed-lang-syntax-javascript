#include <yed/plugin.h>
#include <yed/highlight.h>

#define ARRAY_LOOP(a) for (__typeof((a)[0]) *it = (a); it < (a) + (sizeof(a) / sizeof((a)[0])); ++it)

highlight_info hinfo;

void unload(yed_plugin *self);
void syntax_js_line_handler(yed_event *event);
void syntax_js_frame_handler(yed_event *event);
void syntax_js_buff_mod_pre_handler(yed_event *event);
void syntax_js_buff_mod_post_handler(yed_event *event);


int yed_plugin_boot(yed_plugin *self) {
    yed_event_handler frame, line, buff_mod_pre, buff_mod_post;
    char              *kwds[] = {
        "abstract", "arguments", "await",
        "class", "const",
        "debugger", "delete",
        "enum", "eval", "export", "extends",
        "final", "finally", "function",
        "implements", "import", "in", "instanceof", "interface",
        "let",
        "native", "new",
        "package", "private", "protected", "public",
        "static", "super", "synchronized",
        "transient", "typeof",
        "var", "volatile",
    };
    char              *control_flow[] = {
        "break",
        "case", "catch", "continue",
        "default", "do",
        "else",
        "for",
        "goto",
        "if",
        "return",
        "switch",
        "throw", "throws", "try",
        "while",
        "yield",
    };
    char              *typenames[] = {
        "boolean", "byte",
        "char",
        "double",
        "float",
        "long",
        "int",
        "short",
        "unsigned",
        "void",
    };
    char              *other[] = {
        "Array",    "Date",
        "hasOwnProperty",    "Infinity",    "isFinite",    "isNaN",
        "isPrototypeOf",    "length",    "Math",    "NaN",
        "name",    "Number",    "Object",    "prototype",
        "String",    "toString",    "undefined",    "valueOf",
        "onblur",    "onclick",    "onerror",    "onfocus",
        "onkeydown",    "onkeypress",    "onkeyup",    "onmouseover",
        "onload",    "onmouseup",    "onmousedown",    "onsubmit",
        "alert",    "all",    "anchor",    "anchors",
        "area",    "assign",    "blur",    "button",
        "checkbox",    "clearInterval",    "clearTimeout",    "clientInformation",
        "close",    "closed",    "confirm",    "constructor",
        "crypto",    "decodeURI",    "decodeURIComponent",    "defaultStatus",
        "document",    "element",    "elements",    "embed",
        "embeds",    "encodeURI",    "encodeURIComponent",    "escape",
        "event",    "fileUpload",    "focus",    "form",
        "forms",    "frame",    "innerHeight",    "innerWidth",
        "layer",    "layers",    "link",    "location",
        "mimeTypes",    "navigate",    "navigator",    "frames",
        "frameRate",    "hidden",    "history",    "image",
        "images",    "offscreenBuffering",    "open",    "opener",
        "option",    "outerHeight",    "outerWidth",    "packages",
        "pageXOffset",    "pageYOffset",    "parent",    "parseFloat",
        "parseInt",    "password",    "pkcs11",    "plugin",
        "prompt",    "propertyIsEnum",    "radio",    "reset",
        "screenX",    "screenY",    "scroll",    "secure",
        "select",    "self",    "setInterval",    "setTimeout",
        "status",    "submit",    "taint",    "text",
        "textarea",    "top",    "unescape",    "untaint",
        "window", "true", "false", "this", "null",
    };

    YED_PLUG_VERSION_CHECK();

    yed_plugin_set_unload_fn(self, unload);


    frame.kind          = EVENT_FRAME_PRE_BUFF_DRAW;
    frame.fn            = syntax_js_frame_handler;
    line.kind           = EVENT_LINE_PRE_DRAW;
    line.fn             = syntax_js_line_handler;
    buff_mod_pre.kind   = EVENT_BUFFER_PRE_MOD;
    buff_mod_pre.fn     = syntax_js_buff_mod_pre_handler;
    buff_mod_post.kind  = EVENT_BUFFER_POST_MOD;
    buff_mod_post.fn    = syntax_js_buff_mod_post_handler;

    yed_plugin_add_event_handler(self, frame);
    yed_plugin_add_event_handler(self, line);
    yed_plugin_add_event_handler(self, buff_mod_pre);
    yed_plugin_add_event_handler(self, buff_mod_post);


    highlight_info_make(&hinfo);

    ARRAY_LOOP(kwds)
        highlight_add_kwd(&hinfo, *it, HL_KEY);
    ARRAY_LOOP(control_flow)
        highlight_add_kwd(&hinfo, *it, HL_CF);
    ARRAY_LOOP(typenames)
        highlight_add_kwd(&hinfo, *it, HL_TYPE);
    ARRAY_LOOP(other)
        highlight_add_kwd(&hinfo, *it, HL_CON);
    highlight_suffixed_words(&hinfo, '(', HL_CALL);
    highlight_numbers(&hinfo);
    highlight_within(&hinfo, "\"", "\"", '\\', -1, HL_STR);
    highlight_within(&hinfo, "'", "'", '\\', -1, HL_STR);
    highlight_to_eol_from(&hinfo, "//", HL_COMMENT);
    highlight_within_multiline(&hinfo, "/*", "*/", 0, HL_COMMENT);

    ys->redraw = 1;

    return 0;
}

void unload(yed_plugin *self) {
    highlight_info_free(&hinfo);
    ys->redraw = 1;
}

void syntax_js_frame_handler(yed_event *event) {
    yed_frame *frame;

    frame = event->frame;

    if (!frame
    ||  !frame->buffer
    ||  frame->buffer->kind != BUFF_KIND_FILE
    ||  frame->buffer->ft != yed_get_ft("JavaScript")) {
        return;
    }

    highlight_frame_pre_draw_update(&hinfo, event);
}

void syntax_js_line_handler(yed_event *event) {
    yed_frame *frame;

    frame = event->frame;

    if (!frame
    ||  !frame->buffer
    ||  frame->buffer->kind != BUFF_KIND_FILE
    ||  frame->buffer->ft != yed_get_ft("JavaScript")) {
        return;
    }

    highlight_line(&hinfo, event);
}

void syntax_js_buff_mod_pre_handler(yed_event *event) {
    if (event->buffer == NULL
    ||  event->buffer->kind != BUFF_KIND_FILE
    ||  event->buffer->ft != yed_get_ft("JavaScript")) {
        return;
    }

    highlight_buffer_pre_mod_update(&hinfo, event);
}

void syntax_js_buff_mod_post_handler(yed_event *event) {
    if (event->buffer == NULL
    ||  event->buffer->kind != BUFF_KIND_FILE
    ||  event->buffer->ft != yed_get_ft("JavaScript")) {
        return;
    }

    highlight_buffer_post_mod_update(&hinfo, event);
}
