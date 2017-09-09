var __extends = (this && this.__extends) || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
};
var bbox;
(function (bbox) {
    var ds;
    (function (ds) {
        var Map = (function () {
            function Map() {
                this.map = {};
            }
            Map.prototype.has = function (key) {
                return this.map.hasOwnProperty((key));
            };
            Map.prototype.set = function (key, value) {
                (this.map)[(key)] = (value);
                return this;
            };
            Map.prototype.get = function (key) {
                if (!this.map.hasOwnProperty((key)))
                    return undefined;
                return (this.map)[(key)];
            };
            Map.prototype.forEach = function (func, this_inst) {
                var passed_this = this_inst;
                if (!passed_this)
                    passed_this = this;
                for (var key in (this.map)) {
                    func.apply(passed_this, [(key), ((this.map)[key])]);
                }
                return this;
            };
            return Map;
        }());
        ds.Map = Map;
    })(ds = bbox.ds || (bbox.ds = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var ds;
    (function (ds) {
        var Deque = (function () {
            function Deque() {
                this.array = [];
            }
            Deque.prototype.empty = function () {
                return this.array.length == 0;
            };
            Deque.prototype.size = function () {
                return this.array.length;
            };
            Deque.prototype.back = function () {
                console.assert(this.array.length > 0, "bbox.ds.Deque.back() called but empty");
                return this.array[this.array.length - 1];
            };
            Deque.prototype.front = function () {
                console.assert(this.array.length > 0, "bbox.ds.Deque.front() called but empty");
                return this.array[0];
            };
            Deque.prototype.at = function (index) {
                console.assert((index >= 0) && (index < this.array.length), "bbox.ds.Deque.at() called with invalid index " + index + ", expected 0.." + (this.array.length - 1));
                return this.array[index];
            };
            Deque.prototype.push_back = function (val) {
                this.array.push(val);
            };
            Deque.prototype.pop_back = function () {
                console.assert(this.array.length > 0, "bbox.ds.Deque.pop_back() called but empty");
                return this.array.pop();
            };
            Deque.prototype.pop_front = function () {
                console.assert(this.array.length > 0, "bbox.ds.Deque.pop_front() called but empty");
                return this.array.shift();
            };
            Deque.prototype.clear = function () {
                this.array = [];
            };
            Deque.prototype.shallowClone = function () {
                var result = new Deque();
                for (var i = 0; i < this.array.length; ++i) {
                    result.array.push(this.array[i]);
                }
                return result;
            };
            return Deque;
        }());
        ds.Deque = Deque;
    })(ds = bbox.ds || (bbox.ds = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var ui;
    (function (ui) {
        var Timer = (function () {
            function Timer(handler) {
                this.handler = handler;
                this.is_started = false;
            }
            Timer.prototype.isRunning = function () {
                return this.is_started;
            };
            Timer.prototype.stop = function () {
                if (this.is_started) {
                    this.is_started = false;
                    if (this.is_periodic)
                        window.clearInterval(this.id);
                    else
                        window.clearTimeout(this.id);
                }
            };
            Timer.prototype.startSingleShot = function (timeout_ms) {
                this.stop();
                this.is_started = true;
                this.is_periodic = false;
                this.want_periodic = false;
                this.id = window.setTimeout(this.doTimeout.bind(this), timeout_ms);
            };
            Timer.prototype.startPeriodic = function (initial_ms, period_ms) {
                this.stop();
                this.is_started = true;
                this.is_periodic = false;
                this.want_periodic = true;
                this.period = period_ms;
                this.id = window.setTimeout(this.doTimeout.bind(this), initial_ms);
            };
            Timer.prototype.doTimeout = function () {
                if (!this.is_periodic) {
                    if (this.want_periodic) {
                        this.is_periodic = true;
                        this.id = window.setInterval(this.doTimeout.bind(this), this.period);
                    }
                    else {
                        this.is_started = false;
                    }
                }
                this.handler();
            };
            return Timer;
        }());
        ui.Timer = Timer;
    })(ui = bbox.ui || (bbox.ui = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var ui;
    (function (ui) {
        var Control = (function () {
            function Control(element) {
                this.m_control_element = element;
            }
            Control.prototype.htmlElement = function () {
                return this.m_control_element;
            };
            Control.prototype.addClass = function (class_name) {
                this.m_control_element.classList.add(class_name);
            };
            Control.prototype.removeClass = function (class_name) {
                this.m_control_element.classList.remove(class_name);
            };
            return Control;
        }());
        ui.Control = Control;
    })(ui = bbox.ui || (bbox.ui = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var ui;
    (function (ui) {
        var Container = (function (_super) {
            __extends(Container, _super);
            function Container(element) {
                _super.call(this, element);
                this.m_container_element = element;
            }
            Container.prototype.add = function (control) {
                this.m_container_element.appendChild(control.htmlElement());
                return this;
            };
            return Container;
        }(ui.Control));
        ui.Container = Container;
    })(ui = bbox.ui || (bbox.ui = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var ui;
    (function (ui) {
        var BodyContainer = (function (_super) {
            __extends(BodyContainer, _super);
            function BodyContainer() {
                _super.call(this, window.document.body);
            }
            return BodyContainer;
        }(ui.Container));
        var Application = (function () {
            function Application(state_prototype) {
                var _this = this;
                this.m_type_prototype = state_prototype;
                window.bbox_ui_application = this;
                this.m_body = new BodyContainer();
                window.onload = function (ev) {
                    var state = null;
                    if (history.state
                        && (typeof (history.state) === "string")) {
                        state = _this.xmlToState(history.state);
                    }
                    if (state === null) {
                        state = _this.decodeUrlToState(window.location.pathname);
                    }
                    if (state === null) {
                        state = new _this.m_type_prototype();
                    }
                    _this.onLoad(state);
                };
                window.onpopstate = function (ev) {
                    var state = null;
                    if (ev.state
                        && (typeof (ev.state) === "string")) {
                        state = _this.xmlToState(ev.state);
                    }
                    if (state === null) {
                        state = _this.decodeUrlToState(window.location.pathname);
                    }
                    if (state === null) {
                        state = new _this.m_type_prototype();
                    }
                    _this.onPopState(state);
                };
            }
            Application.prototype.body = function () {
                return this.m_body;
            };
            Application.prototype.pushState = function (url, state) {
                var xml = this.stateToXml(state);
                history.pushState(xml, "", url);
                this.onPopState(state);
            };
            Application.prototype.replaceState = function (url, state) {
                var xml = this.stateToXml(state);
                history.replaceState(xml, "", url);
                this.onPopState(state);
            };
            Application.prototype.stateToXml = function (state) {
                if ((state === null)
                    || (state === undefined)) {
                    return null;
                }
                var enc = new bbox.enc.ToXml("state");
                this.m_type_prototype.type.toXml(state, enc);
                if (enc.hasError()) {
                    console.log("Could not encode state: " + enc.getErrorString());
                    return null;
                }
                return enc.getXmlString();
            };
            Application.prototype.xmlToState = function (xml) {
                if ((xml === null)
                    || (xml === undefined)
                    || (typeof (xml) != "string")
                    || (xml == "")) {
                    return null;
                }
                var enc = new bbox.enc.FromXml(xml, "state");
                var state = this.m_type_prototype.type.fromXml(enc);
                if (enc.hasError()) {
                    console.log("Could nto decode state: " + enc.getErrorString());
                    return null;
                }
                return state;
            };
            return Application;
        }());
        ui.Application = Application;
    })(ui = bbox.ui || (bbox.ui = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var ui;
    (function (ui) {
        var TextControl = (function (_super) {
            __extends(TextControl, _super);
            function TextControl(element) {
                _super.call(this, element);
            }
            TextControl.prototype.setText = function (text) {
                this.htmlElement().textContent = text;
                return this;
            };
            return TextControl;
        }(ui.Control));
        ui.TextControl = TextControl;
    })(ui = bbox.ui || (bbox.ui = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var ui;
    (function (ui) {
        var EditBox = (function (_super) {
            __extends(EditBox, _super);
            function EditBox(text) {
                var _this = this;
                var input = document.createElement("input");
                input.value = text;
                input.type = 'text';
                _super.call(this, input);
                this.m_input_element = input;
                this.m_on_change = null;
                input.oninput = function () {
                    _this.handleChange();
                };
            }
            EditBox.prototype.onChanged = function (handler) {
                this.m_on_change = handler;
                return this;
            };
            EditBox.prototype.getValue = function () {
                return this.m_input_element.value;
            };
            EditBox.prototype.setValue = function (new_value) {
                this.m_input_element.value = new_value;
            };
            EditBox.prototype.handleChange = function () {
                if (this.m_on_change) {
                    this.m_on_change();
                }
            };
            return EditBox;
        }(ui.TextControl));
        ui.EditBox = EditBox;
    })(ui = bbox.ui || (bbox.ui = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var ui;
    (function (ui) {
        var Button = (function (_super) {
            __extends(Button, _super);
            function Button(text) {
                var _this = this;
                var button = document.createElement("button");
                button.textContent = text;
                _super.call(this, button);
                this.m_button_element = button;
                this.m_on_click = null;
                button.onclick = function () {
                    _this.handleClick();
                };
            }
            Button.prototype.onClick = function (handler) {
                this.m_on_click = handler;
                return this;
            };
            Button.prototype.handleClick = function () {
                if (this.m_on_click) {
                    this.m_on_click();
                }
            };
            return Button;
        }(ui.TextControl));
        ui.Button = Button;
    })(ui = bbox.ui || (bbox.ui = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var ui;
    (function (ui) {
        var Span = (function (_super) {
            __extends(Span, _super);
            function Span(text) {
                var span = document.createElement("span");
                span.textContent = text;
                _super.call(this, span);
                this.m_span_element = span;
            }
            return Span;
        }(ui.TextControl));
        ui.Span = Span;
    })(ui = bbox.ui || (bbox.ui = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var ui;
    (function (ui) {
        var Div = (function (_super) {
            __extends(Div, _super);
            function Div() {
                var div = document.createElement("div");
                _super.call(this, div);
                this.m_div_element = div;
            }
            return Div;
        }(ui.Container));
        ui.Div = Div;
    })(ui = bbox.ui || (bbox.ui = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var ui;
    (function (ui) {
        var Pre = (function (_super) {
            __extends(Pre, _super);
            function Pre(text) {
                var pre = document.createElement("pre");
                pre.textContent = text;
                _super.call(this, pre);
                this.m_pre_element = pre;
            }
            Pre.prototype.appendText = function (text) {
                this.m_pre_element.appendChild(document.createTextNode(text));
            };
            return Pre;
        }(ui.TextControl));
        ui.Pre = Pre;
    })(ui = bbox.ui || (bbox.ui = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var net;
    (function (net) {
        var AjaxRequest = (function () {
            function AjaxRequest(method, url, handler) {
                var _this = this;
                this.m_sent = false;
                this.m_aborted = false;
                this.m_success = false;
                this.m_handled = false;
                this.m_handler = handler;
                this.m_request = new XMLHttpRequest();
                this.m_request.open(method, url, true);
                this.m_request.onload = function (event) {
                    _this.handleComplete(true);
                };
                this.m_request.onerror = function (event) {
                    _this.handleComplete(false);
                };
            }
            AjaxRequest.prototype.isSent = function () {
                return this.m_sent;
            };
            AjaxRequest.prototype.isComplete = function () {
                return this.m_request.readyState == XMLHttpRequest.DONE;
            };
            AjaxRequest.prototype.wasSuccessful = function () {
                return this.isComplete() && this.m_success;
            };
            AjaxRequest.prototype.sendAsync = function () {
                console.assert(!this.m_sent, "bbox.net.AjaxRequest: Cannot call sendAsync when already sent");
                this.m_sent = true;
                this.m_request.send();
            };
            AjaxRequest.prototype.sendAsyncWithContent = function (content) {
                console.assert(!this.m_sent, "bbox.net.AjaxRequest: Cannot call sendAsync when already sent");
                this.m_sent = true;
                this.m_request.send(content);
            };
            AjaxRequest.prototype.abortAsync = function () {
                console.assert(this.m_sent, "bbox.net.AjaxRequest: Cannot call abortAsync before calling sendAsync");
                if (!this.m_aborted) {
                    this.m_aborted = true;
                    this.m_request.abort();
                }
            };
            AjaxRequest.prototype.getResponseAsText = function () {
                console.assert(this.m_handled, "bbox.net.AjaxRequest: Cannot call getResponseAsXmlDocument before request is completed");
                console.assert(this.m_success, "bbox.net.AjaxRequest: Cannot call getResponseAsXmlDocument before on failed request");
                return this.m_request.responseText;
            };
            AjaxRequest.prototype.getResponseAsXmlDocument = function () {
                console.assert(this.m_handled, "bbox.net.AjaxRequest: Cannot call getResponseAsXmlDocument before request is completed");
                console.assert(this.m_success, "bbox.net.AjaxRequest: Cannot call getResponseAsXmlDocument before on failed request");
                return this.m_request.responseXML;
            };
            AjaxRequest.prototype.getResponseAsJson = function () {
                console.assert(this.m_handled, "bbox.net.AjaxRequest: Cannot call getResponseAsJson before request is completed");
                console.assert(this.m_success, "bbox.net.AjaxRequest: Cannot call getResponseAsJson before on failed request");
                return JSON.parse(this.m_request.responseText);
            };
            AjaxRequest.prototype.getErrorDescription = function () {
                console.assert(this.m_handled, "bbox.net.AjaxRequest: Cannot call getErrorDescription before request is completed");
                console.assert(this.m_success, "bbox.net.AjaxRequest: Cannot call getErrorDescription before on successful request");
                return this.m_request.statusText;
            };
            AjaxRequest.prototype.handleComplete = function (success) {
                if (!this.m_aborted
                    && !this.m_handled) {
                    this.m_handled = true;
                    this.m_success = success;
                    if (this.m_handler) {
                        this.m_handler(this);
                    }
                }
            };
            return AjaxRequest;
        }());
        net.AjaxRequest = AjaxRequest;
    })(net = bbox.net || (bbox.net = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var net;
    (function (net) {
        var BboxRpcRequest = (function () {
            function BboxRpcRequest(url, method, input_type, output_type, handler) {
                var _this = this;
                this.m_url = url;
                this.m_method = method;
                this.m_input_type = input_type.type;
                this.m_output_type = output_type.type;
                this.m_output = null;
                this.m_handler = handler;
                this.m_is_complete = false;
                this.m_was_successful = false;
                this.m_error_str = null;
                var final_url;
                final_url = url;
                if (final_url.substring(-1) !== "/") {
                    final_url = final_url.concat("/");
                }
                final_url = final_url.concat(method);
                this.m_request = new net.AjaxRequest("POST", final_url, function (request) {
                    _this.handleComplete();
                });
            }
            BboxRpcRequest.prototype.sendAsync = function (args) {
                var to_xml = new bbox.enc.ToXml("params");
                this.m_input_type.toXml(args, to_xml);
                this.m_request.sendAsyncWithContent(to_xml.getXmlString());
            };
            BboxRpcRequest.prototype.isComplete = function () {
                return this.m_is_complete;
            };
            BboxRpcRequest.prototype.wasSuccessul = function () {
                console.assert(this.m_is_complete, "bbox.net.BboxRpcRequest: wasSucessful called before complete");
                return this.m_was_successful;
            };
            BboxRpcRequest.prototype.getErrorDescription = function () {
                console.assert(this.m_is_complete, "bbox.net.BboxRpcRequest: getErrorDescription called before complete");
                console.assert(!this.m_was_successful, "bbox.net.BboxRpcRequest: getErrorDescription called on successful request");
                return this.m_error_str;
            };
            BboxRpcRequest.prototype.getResult = function () {
                console.assert(this.m_is_complete, "bbox.net.BboxRpcRequest: getResult called before complete");
                console.assert(this.m_was_successful, "bbox.net.BboxRpcRequest: getResult called on failed request");
                return this.m_output;
            };
            BboxRpcRequest.prototype.handleComplete = function () {
                this.m_is_complete = true;
                if (!this.m_request.wasSuccessful()) {
                    this.m_was_successful = false;
                    this.m_error_str = "Ajax Request Failed: " + this.m_request.getErrorDescription();
                }
                else {
                    try {
                        var from_xml = new bbox.enc.FromXml(this.m_request.getResponseAsText(), "results");
                        this.m_output = this.m_output_type.fromXml(from_xml);
                        if (from_xml.hasError()) {
                            this.m_output = null;
                            this.m_was_successful = false;
                            this.m_error_str = "Error decoding result XML: " + from_xml.getErrorString();
                        }
                        else {
                            this.m_was_successful = true;
                        }
                    }
                    catch (err) {
                        this.m_was_successful = false;
                        this.m_error_str = "Could not decode response as JSON";
                    }
                }
                this.m_handler(this);
            };
            return BboxRpcRequest;
        }());
        net.BboxRpcRequest = BboxRpcRequest;
    })(net = bbox.net || (bbox.net = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var enc;
    (function (enc) {
        var Type = (function () {
            function Type(lib, name) {
                this.type_lib = lib;
                this.type_name = name;
            }
            Type.prototype.getTypeLibrary = function () {
                return this.type_lib;
            };
            Type.prototype.getName = function () {
                return this.type_name;
            };
            return Type;
        }());
        enc.Type = Type;
    })(enc = bbox.enc || (bbox.enc = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var enc;
    (function (enc) {
        var ToString = (function () {
            function ToString() {
                this.result = "";
                this.has_error = false;
                this.error = "";
            }
            ToString.prototype.getStringalue = function () {
                return this.result;
            };
            ToString.prototype.hasError = function () {
                return this.has_error;
            };
            ToString.prototype.getErrorString = function () {
                return this.error;
            };
            ToString.prototype.raiseError = function (err) {
                if (!this.has_error) {
                    this.has_error = true;
                    this.error = err;
                }
            };
            ToString.prototype.append = function (str) {
                this.result += str;
            };
            return ToString;
        }());
        enc.ToString = ToString;
    })(enc = bbox.enc || (bbox.enc = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var enc;
    (function (enc) {
        var ToXml = (function () {
            function ToXml(root_node_name) {
                this.result = document.implementation.createDocument(null, root_node_name, null);
                this.cur_element = this.result.documentElement;
                this.element_stack = [];
                this.has_error = false;
                this.error = "";
                this.state = "Constructed";
            }
            ToXml.prototype.getXmlString = function () {
                return (new XMLSerializer()).serializeToString(this.result);
            };
            ToXml.prototype.hasError = function () {
                return this.has_error;
            };
            ToXml.prototype.getErrorString = function () {
                return this.error;
            };
            ToXml.prototype.raiseError = function (err) {
                if (!this.has_error) {
                    this.has_error = true;
                    this.error = err;
                }
            };
            ToXml.prototype.setValue = function (val) {
                if (this.has_error) {
                }
                else {
                    if (val === null) {
                        this.raiseError("Can't call setValue with null value");
                    }
                    else if (typeof val != "string") {
                        this.raiseError("Can't call setValue wtih value of type " + typeof val);
                    }
                    else if (this.state == "Constructed") {
                        this.cur_element.setAttribute("value", val);
                        this.state = "Completed-Single-Value";
                    }
                    else if (this.state == "Started-Named-Value") {
                        this.cur_element.setAttribute("value", val);
                        this.state = "Completed-Named-Value";
                    }
                    else {
                        this.raiseError("Can't call setValue in state " + this.state);
                    }
                }
            };
            ToXml.prototype.encodeTypedValue = function (val, type_name) {
                if (this.has_error) {
                }
                else {
                    var type = enc.TypeLibrary.instance().findType(type_name);
                    if (!type) {
                        this.raiseError("Unknown type " + type_name);
                    }
                    else {
                        type.toXml(val, this);
                    }
                }
            };
            ToXml.prototype.startObject = function () {
                if (this.has_error) {
                }
                else if (this.state == "Constructed") {
                    this.state = "Started-Single-Object";
                }
                else if (this.state == "Started-Named-Value") {
                    this.state = "Started-Named-Object";
                }
                else {
                    this.raiseError("Can't call startObject in state " + this.state);
                }
            };
            ToXml.prototype.endObject = function () {
                if (this.has_error) {
                }
                else if (this.state == "Started-Single-Object") {
                    this.state = "Completed-Single-Value";
                }
                else if (this.state == "Started-Named-Object") {
                    this.state = "Completed-Named-Value";
                }
                else {
                    this.raiseError("Can't call endObject in state " + this.state);
                }
            };
            ToXml.prototype.startNamedValue = function (name) {
                if (this.has_error) {
                }
                else if ((this.state == "Started-Single-Object")
                    || (this.state == "Started-Named-Object")) {
                    var parent_element = this.cur_element;
                    this.cur_element = this.result.createElement(name);
                    this.element_stack.push(parent_element);
                    parent_element.appendChild(this.cur_element);
                    this.state = "Started-Named-Value";
                }
                else {
                    this.raiseError("Can't call startNamedValue in state " + this.state);
                }
            };
            ToXml.prototype.endNamedValue = function () {
                if (this.has_error) {
                }
                else if (this.state == "Completed-Named-Value") {
                    this.cur_element = this.element_stack.pop();
                    if (this.element_stack.length == 0) {
                        this.state = "Started-Single-Object";
                    }
                    else {
                        this.state = "Started-Named-Object";
                    }
                }
                else {
                    this.raiseError("Can't call endNamedValue in state " + this.state);
                }
            };
            return ToXml;
        }());
        enc.ToXml = ToXml;
    })(enc = bbox.enc || (bbox.enc = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var enc;
    (function (enc) {
        var FromXml_ObjectProgress = (function () {
            function FromXml_ObjectProgress(element) {
                this.element = element;
                this.at_start = true;
                this.finished = false;
                this.cur_child = null;
            }
            FromXml_ObjectProgress.prototype.nextElement = function () {
                var result;
                if (this.finished) {
                    result = null;
                }
                else if (this.at_start) {
                    result = this.element.firstElementChild;
                }
                else {
                    result = this.cur_child.nextElementSibling;
                }
                if (result === null) {
                    this.finished = true;
                }
                else {
                    this.cur_child = result;
                    this.at_start = false;
                }
                return result;
            };
            return FromXml_ObjectProgress;
        }());
        ;
        var FromXml = (function () {
            function FromXml(json_str, root_element_name) {
                this.document = null;
                this.root_element = null;
                this.cur_obj_progress = null;
                this.parent_stack = new bbox.ds.Deque();
                this.has_error = false;
                this.error = "";
                this.state = "Constructed";
                try {
                    var parser = new DOMParser();
                    this.document = parser.parseFromString(json_str, "text/xml");
                    this.root_element = this.document.documentElement;
                }
                catch (e) {
                    this.raiseError("Syntax error parsing XML input");
                }
                if (!this.hasError) {
                    if (this.root_element.nodeName != root_element_name) {
                        this.raiseError("Unexpected root element name - got \"" + this.root_element.nodeName + "\", expected \"" + root_element_name + "\"");
                    }
                }
            }
            FromXml.prototype.hasError = function () {
                return this.has_error;
            };
            FromXml.prototype.getErrorString = function () {
                return this.error;
            };
            FromXml.prototype.raiseError = function (err) {
                if (!this.has_error) {
                    this.has_error = true;
                    this.error = err;
                }
            };
            FromXml.prototype.checkComplete = function () {
                if (this.has_error) {
                }
                else if (this.state != "Complete") {
                    this.raiseError("checkComplete called but in state " + this.state);
                }
            };
            FromXml.prototype.decodeTypedValue = function (type_name) {
                if (this.has_error) {
                    // No-op
                    return undefined;
                }
                else {
                    var type = enc.TypeLibrary.instance().findType(type_name);
                    if (!type) {
                        this.raiseError("Unknown type " + type_name);
                        return undefined;
                    }
                    else {
                        return type.fromXml(this);
                    }
                }
            };
            FromXml.prototype.getValue = function () {
                var result;
                if (this.has_error) {
                    return undefined;
                }
                else if ((this.state == "Constructed")
                    || (this.state == "Started-Named-Value")) {
                    var element;
                    if (this.state == "Constructed")
                        element = this.root_element;
                    else
                        element = this.cur_obj_progress.cur_child;
                    if (element.childElementCount != 0) {
                        this.raiseError("Expected value but found element \"" + element.nodeName
                            + "\" containing " + element.childElementCount + " child elements including \""
                            + element.firstElementChild.nodeName + "\"");
                        return undefined;
                    }
                    else if ((element.textContent != "")
                        && element.hasAttribute("value")) {
                        this.raiseError("Expected value but found element \"" + element.nodeName
                            + "\" containing both text content and a value attribute");
                        return undefined;
                    }
                    else if (element.hasAttribute("value")) {
                        result = element.getAttribute("value");
                    }
                    else {
                        result = element.textContent;
                    }
                    if (this.state == "Constructed")
                        this.state = "Complete";
                    else
                        this.state = "Completed-Named-Value";
                }
                else {
                    this.raiseError("Can't call getValue in state " + this.state);
                    return undefined;
                }
                if (result === null) {
                    this.raiseError("Can't call getValue for null objects - all values must be strings");
                    return undefined;
                }
                else if (typeof result == "number") {
                    // Javascript automatically converts strings which are valid numbers -
                    // just convert back to a string
                    return (result).toString();
                }
                else if (typeof result != "string") {
                    this.raiseError("Can't call getValue for objects of type \"" + typeof result + "\" - all values must be strings");
                    return undefined;
                }
                else {
                    return result;
                }
            };
            FromXml.prototype.startObject = function () {
                var cur_progress;
                if (this.has_error) {
                    // No-op
                    return;
                }
                else if (this.state == "Constructed") {
                    cur_progress = new FromXml_ObjectProgress(this.root_element);
                }
                else if (this.state == "Started-Named-Value") {
                    var cur_progress = new FromXml_ObjectProgress(this.cur_obj_progress.cur_child);
                    this.parent_stack.push_back(this.cur_obj_progress);
                }
                else {
                    this.raiseError("Can't call startObject in state " + this.state);
                    return;
                }
                this.cur_obj_progress = cur_progress;
                this.state = "In-Object";
            };
            FromXml.prototype.endObject = function () {
                if (this.has_error) {
                }
                else if (this.state == "In-Object") {
                    var obj_element = this.cur_obj_progress.element;
                    var remaining_element = this.cur_obj_progress.nextElement();
                    if (remaining_element !== null) {
                        this.raiseError("endObject called, but element \"" + obj_element.nodeName + "\" contains more elements, including \""
                            + remaining_element.nodeName + "\"");
                        return;
                    }
                    if (this.parent_stack.empty()) {
                        // Finished root object
                        this.state = "Complete";
                    }
                    else {
                        // Finished sub-object
                        this.cur_obj_progress = this.parent_stack.pop_back();
                        this.state = "Completed-Named-Value";
                    }
                }
                else {
                    this.raiseError("Can't call endObject in state " + this.state);
                    return;
                }
            };
            FromXml.prototype.startNamedValue = function (name) {
                if (this.has_error) {
                }
                else if (this.state == "In-Object") {
                    var parent = this.cur_obj_progress.element;
                    var element = this.cur_obj_progress.nextElement();
                    if (element === null) {
                        this.raiseError("startNamedValue called in element \"" + parent.nodeName
                            + "\" expecting element \"" + name + "\" but no elements were remaining");
                    }
                    else if (element.nodeName != name) {
                        this.raiseError("startNamedValue called in element \"" + parent.nodeName
                            + "\" expecting element \"" + name + "\" but got element \"" + element.nodeName + "\"");
                    }
                    else {
                        this.state = "Started-Named-Value";
                    }
                }
                else {
                    this.raiseError("Can't call startNamedValue in state " + this.state);
                    return;
                }
            };
            FromXml.prototype.endNamedValue = function () {
                if (this.has_error) {
                }
                else if (this.state == "Completed-Named-Value") {
                    this.state = "In-Object";
                }
                else {
                    this.raiseError("Can't call endNamedValue in state " + this.state);
                    return;
                }
            };
            return FromXml;
        }());
        enc.FromXml = FromXml;
    })(enc = bbox.enc || (bbox.enc = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var enc;
    (function (enc) {
        var details;
        (function (details) {
            var IntType = (function (_super) {
                __extends(IntType, _super);
                function IntType(lib, name, min, max) {
                    _super.call(this, lib, name);
                    this.min = min;
                    this.max = max;
                }
                IntType.prototype.isValue = function () {
                    return true;
                };
                IntType.prototype.toString = function (val, encoder) {
                    if (typeof val != "number") {
                        encoder.raiseError("Can't encode value of type \"" + typeof val + "\" as integer type " + this.getName());
                        return;
                    }
                    if (!((val >= this.min)
                        && (val <= this.max))) {
                        encoder.raiseError("Can't encode value \"" + val + "\" as integer type " + this.getName() + " because it's not in the range "
                            + this.min + ".." + this.max);
                        return;
                    }
                    encoder.append((val).toString());
                };
                IntType.prototype.toXml = function (val, encoder) {
                    if (typeof val != "number") {
                        encoder.raiseError("Can't encode value of type \"" + typeof val + "\" as integer type " + this.getName());
                        return;
                    }
                    if (!((val >= this.min)
                        && (val <= this.max))) {
                        encoder.raiseError("Can't encode value \"" + val + "\" as integer type " + this.getName() + " because it's not in the range "
                            + this.min + ".." + this.max);
                        return;
                    }
                    encoder.setValue(val.toString());
                };
                IntType.prototype.fromXml = function (decoder) {
                    var str = decoder.getValue();
                    if (decoder.hasError())
                        return undefined;
                    if (typeof str != "string") {
                        decoder.raiseError("Can't decode value of type \"" + typeof str + "\" as integer type " + this.getName() + " - must be encoded in JSON as a string");
                        return;
                    }
                    var result = parseInt(str);
                    if (!((result >= this.min)
                        && (result <= this.max))) {
                        decoder.raiseError("Can't decode value \"" + str + "\" as integer type " + this.getName() + " because it's not in the range "
                            + this.min + ".." + this.max);
                        return undefined;
                    }
                    return result;
                };
                return IntType;
            }(enc.Type));
            details.IntType = IntType;
        })(details = enc.details || (enc.details = {}));
    })(enc = bbox.enc || (bbox.enc = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var enc;
    (function (enc) {
        var details;
        (function (details) {
            var StringType = (function (_super) {
                __extends(StringType, _super);
                function StringType(lib) {
                    _super.call(this, lib, "std::string");
                }
                StringType.prototype.isValue = function () {
                    return true;
                };
                StringType.prototype.toString = function (val, encoder) {
                    if (typeof val != "string") {
                        encoder.raiseError("Can't encode value of type \"" + typeof val + "\" as string");
                        return;
                    }
                    encoder.append(JSON.stringify(val));
                };
                StringType.prototype.toXml = function (val, encoder) {
                    if (typeof val != "string") {
                        encoder.raiseError("Can't encode value of type \"" + typeof val + "\" as string");
                        return;
                    }
                    encoder.setValue(val);
                };
                StringType.prototype.fromXml = function (decoder) {
                    var str = decoder.getValue();
                    if (decoder.hasError())
                        return undefined;
                    if (typeof str != "string") {
                        decoder.raiseError("Can't decode value of type \"" + typeof str + "\" as string");
                        return;
                    }
                    return str;
                };
                return StringType;
            }(enc.Type));
            details.StringType = StringType;
        })(details = enc.details || (enc.details = {}));
    })(enc = bbox.enc || (bbox.enc = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var enc;
    (function (enc) {
        var details;
        (function (details) {
            var SimpleStructureType = (function (_super) {
                __extends(SimpleStructureType, _super);
                function SimpleStructureType(lib, name) {
                    _super.call(this, lib, name);
                    this.members = new bbox.ds.Map();
                }
                SimpleStructureType.prototype.addMemberByType = function (member_name, member_type) {
                    if (this.members.has(member_name)) {
                        console.assert(false, "bbox.enc.Type " + this.getName() + " already has member with name " + member_name);
                        return this;
                    }
                    this.members.set(member_name, member_type);
                    return this;
                };
                SimpleStructureType.prototype.addMember = function (member_name, type_name) {
                    var type = this.getTypeLibrary().findType(type_name);
                    if (type) {
                        this.addMemberByType(member_name, type);
                    }
                    return this;
                };
                SimpleStructureType.prototype.isValue = function () {
                    return false;
                };
                SimpleStructureType.prototype.toString = function (val, encoder) {
                    var _this = this;
                    if (typeof val != "object") {
                        encoder.raiseError("Can't encode value of type \"" + typeof val + "\" as structure " + this.getName());
                        return;
                    }
                    if (val == null) {
                        encoder.raiseError("Can't encode null value as structure " + this.getName());
                        return;
                    }
                    encoder.append("{");
                    var prefix = "";
                    this.members.forEach(function (key, type) {
                        if (!(val).hasOwnProperty(key)) {
                            encoder.raiseError("Object of type " + typeof (val) + " encoding as " + _this.getName() + " seems to have no member " + key);
                            return;
                        }
                        encoder.append(prefix);
                        prefix = ", ";
                        encoder.append(key);
                        encoder.append("=");
                        type.toString(val[key], encoder);
                    });
                    encoder.append("}");
                };
                SimpleStructureType.prototype.toXml = function (val, encoder) {
                    var _this = this;
                    if (typeof val != "object") {
                        encoder.raiseError("Can't encode value of type \"" + typeof val + "\" as structure " + this.getName());
                        return;
                    }
                    if (val == null) {
                        encoder.raiseError("Can't encode null value as structure " + this.getName());
                        return;
                    }
                    encoder.startObject();
                    this.members.forEach(function (key, type) {
                        if (!(val).hasOwnProperty(key)) {
                            encoder.raiseError("Object of type " + typeof (val) + " encoding as " + _this.getName() + " seems to have no member " + key);
                            return;
                        }
                        encoder.startNamedValue(key);
                        type.toXml(val[key], encoder);
                        encoder.endNamedValue();
                    });
                    encoder.endObject();
                };
                SimpleStructureType.prototype.fromXml = function (decoder) {
                    var result = this.constructDefaultValue();
                    decoder.startObject();
                    if (decoder.hasError())
                        return undefined;
                    this.members.forEach(function (key, type) {
                        decoder.startNamedValue(key);
                        result[key] = type.fromXml(decoder);
                        decoder.endNamedValue();
                        if (decoder.hasError())
                            return undefined;
                    });
                    decoder.endObject();
                    if (decoder.hasError())
                        return undefined;
                    return result;
                };
                return SimpleStructureType;
            }(enc.Type));
            details.SimpleStructureType = SimpleStructureType;
            var GenericSimpleStructureType = (function (_super) {
                __extends(GenericSimpleStructureType, _super);
                function GenericSimpleStructureType(lib, name, default_constructor) {
                    _super.call(this, lib, name);
                    this.default_constructor = default_constructor;
                }
                GenericSimpleStructureType.prototype.constructDefaultValue = function () {
                    return new this.default_constructor();
                };
                return GenericSimpleStructureType;
            }(SimpleStructureType));
            details.GenericSimpleStructureType = GenericSimpleStructureType;
        })(details = enc.details || (enc.details = {}));
    })(enc = bbox.enc || (bbox.enc = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var enc;
    (function (enc) {
        var details;
        (function (details) {
            var DequeType = (function (_super) {
                __extends(DequeType, _super);
                function DequeType(lib, name, member_type) {
                    _super.call(this, lib, name);
                    this.member_type = member_type;
                    this.uint32_type = lib.findType("uint32_t");
                }
                DequeType.prototype.isValue = function () {
                    return false;
                };
                DequeType.prototype.toString = function (val, encoder) {
                    if (typeof val != "object") {
                        encoder.raiseError("Can't encode value of type \"" + typeof val + "\" as vector/deque " + this.getName());
                        return;
                    }
                    var deque = (val);
                    encoder.append("[");
                    var prefix = "";
                    for (var i = 0; i < deque.size(); ++i) {
                        encoder.append(prefix);
                        prefix = ", ";
                        this.member_type.toString(deque.at(i), encoder);
                    }
                    encoder.append("]");
                };
                DequeType.prototype.toXml = function (val, encoder) {
                    if (typeof val != "object") {
                        encoder.raiseError("Can't encode value of type \"" + typeof val + "\" as vector/deque " + this.getName());
                        return;
                    }
                    var deque = (val);
                    encoder.startObject();
                    encoder.startNamedValue("size");
                    this.uint32_type.toXml(deque.size(), encoder);
                    encoder.endNamedValue();
                    for (var i = 0; i < deque.size(); ++i) {
                        encoder.startNamedValue("entry");
                        this.member_type.toXml(deque.at(i), encoder);
                        encoder.endNamedValue();
                    }
                    encoder.endObject();
                };
                DequeType.prototype.fromXml = function (decoder) {
                    var result = new bbox.ds.Deque();
                    decoder.startObject();
                    decoder.startNamedValue("size");
                    var size = this.uint32_type.fromXml(decoder);
                    decoder.endNamedValue();
                    var count = 0;
                    while (!decoder.hasError()
                        && (count < size)) {
                        decoder.startNamedValue("entry");
                        result.push_back(this.member_type.fromXml(decoder));
                        decoder.endNamedValue();
                        count += 1;
                    }
                    decoder.endObject();
                    return result;
                };
                return DequeType;
            }(enc.Type));
            details.DequeType = DequeType;
        })(details = enc.details || (enc.details = {}));
    })(enc = bbox.enc || (bbox.enc = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var enc;
    (function (enc) {
        var TypeLibrary = (function () {
            function TypeLibrary() {
                this.by_name = new bbox.ds.Map();
                this.addType(new enc.details.StringType(this));
                this.addType(new enc.details.IntType(this, "uint8_t", 0, 255));
                this.addType(new enc.details.IntType(this, "uint16_t", 0, 65535));
                this.addType(new enc.details.IntType(this, "uint32_t", 0, 4294967295));
                this.addType(new enc.details.IntType(this, "uint64_t", 0, parseInt("FFFFFFFFFFFFFFFF", 16)));
                this.addType(new enc.details.IntType(this, "int8_t", -128, 127));
                this.addType(new enc.details.IntType(this, "int16_t", -32768, 32767));
                this.addType(new enc.details.IntType(this, "int32_t", -21474836478, 2147483647));
                this.addType(new enc.details.IntType(this, "int64_t", parseInt("-9223372036854775808"), parseInt("9223372036854775807")));
            }
            TypeLibrary.findType = function (name) {
                return TypeLibrary.instance().findType(name);
            };
            TypeLibrary.simpleStructure = function (name, type_constructor) {
                return TypeLibrary.instance().simpleStructure(name, type_constructor);
            };
            TypeLibrary.stdVectorAsDeque = function (member_type_name) {
                return TypeLibrary.instance().stdVectorAsDeque(member_type_name);
            };
            TypeLibrary.instance = function () {
                if (!TypeLibrary._instance)
                    TypeLibrary._instance = new TypeLibrary();
                return TypeLibrary._instance;
            };
            TypeLibrary.prototype.findType = function (name) {
                if (!this.by_name.has(name)) {
                    console.assert(false, "bbox.enc.TypeLibrary - no such type " + name);
                    return null;
                }
                return this.by_name.get(name);
            };
            TypeLibrary.prototype.simpleStructure = function (name, type_constructor) {
                var result = new enc.details.GenericSimpleStructureType(this, name, type_constructor);
                this.addType(result);
                return result;
            };
            TypeLibrary.prototype.stdVectorAsDeque = function (member_type_name) {
                var type_name = "std::vector<" + member_type_name + ">";
                if (this.by_name.has(type_name))
                    return this.by_name.get(type_name);
                // First time it was called - create and
                // add a new type for this vector type
                var result = new enc.details.DequeType(this, type_name, this.findType(member_type_name));
                this.addType(result);
                return result;
            };
            TypeLibrary.prototype.addType = function (type) {
                var name = type.getName();
                if (this.by_name.has(name)) {
                    console.assert(false, "bbox.enc.TypeLibrary - duplicate type " + name + " defined");
                }
                else if (type.getTypeLibrary() !== this) {
                    console.assert(false, "bbox.enc.TypeLibrary - adding type that is not owned by this type library");
                }
                else {
                    this.by_name.set(name, type);
                }
            };
            TypeLibrary._instance = null;
            return TypeLibrary;
        }());
        enc.TypeLibrary = TypeLibrary;
    })(enc = bbox.enc || (bbox.enc = {}));
})(bbox || (bbox = {}));
/// <reference path="bbox/ds/Map.ts" />
/// <reference path="bbox/ds/Deque.ts" />
/// <reference path="bbox/ui/Timer.ts" />
/// <reference path="bbox/ui/Control.ts" />
/// <reference path="bbox/ui/Container.ts" />
/// <reference path="bbox/ui/Application.ts" />
/// <reference path="bbox/ui/TextControl.ts" />
/// <reference path="bbox/ui/EditBox.ts" />
/// <reference path="bbox/ui/Button.ts" />
/// <reference path="bbox/ui/Span.ts" />
/// <reference path="bbox/ui/Div.ts" />
/// <reference path="bbox/ui/Pre.ts" />
/// <reference path="bbox/net/AjaxRequest.ts" />
/// <reference path="bbox/net/BboxRpcRequest.ts" />
/// <reference path="bbox/enc/Type.ts" />
/// <reference path="bbox/enc/TypePrototype.ts" />
/// <reference path="bbox/enc/ToString.ts" />
/// <reference path="bbox/enc/ToXml.ts" />
/// <reference path="bbox/enc/FromXml.ts" />
/// <reference path="bbox/enc/details/IntType.ts" />
/// <reference path="bbox/enc/details/StringType.ts" />
/// <reference path="bbox/enc/details/SimpleStructureType.ts" />
/// <reference path="bbox/enc/details/DequeType.ts" />
/// <reference path="bbox/enc/TypeLibrary.ts" />
var bbox;
(function (bbox) {
    var debug;
    (function (debug) {
        var api;
        (function (api) {
            var ChildEntry = (function () {
                function ChildEntry() {
                    this.name = "";
                    this.path = "";
                    this.short_description = "";
                }
                ChildEntry.type = bbox.enc.TypeLibrary.simpleStructure("bbox::debug::api::ChildEntry", ChildEntry)
                    .addMember("name", "std::string")
                    .addMember("path", "std::string")
                    .addMember("short_description", "std::string");
                return ChildEntry;
            }());
            api.ChildEntry = ChildEntry;
            ;
        })(api = debug.api || (debug.api = {}));
    })(debug = bbox.debug || (bbox.debug = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var debug;
    (function (debug) {
        var api;
        (function (api) {
            var DebugReport = (function () {
                function DebugReport() {
                    this.contents_html = "";
                }
                DebugReport.type = bbox.enc.TypeLibrary.simpleStructure("bbox::debug::api::DebugReport", DebugReport)
                    .addMember("contents_html", "std::string");
                return DebugReport;
            }());
            api.DebugReport = DebugReport;
            ;
        })(api = debug.api || (debug.api = {}));
    })(debug = bbox.debug || (bbox.debug = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var debug;
    (function (debug) {
        var api;
        (function (api) {
            var QueryRequest = (function () {
                function QueryRequest() {
                    this.query = "";
                }
                QueryRequest.type = bbox.enc.TypeLibrary.simpleStructure("bbox::debug::api::QueryRequest", QueryRequest)
                    .addMember("query", "std::string");
                return QueryRequest;
            }());
            api.QueryRequest = QueryRequest;
            ;
        })(api = debug.api || (debug.api = {}));
    })(debug = bbox.debug || (bbox.debug = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var debug;
    (function (debug) {
        var api;
        (function (api) {
            var QueryResponseEntry = (function () {
                function QueryResponseEntry() {
                    this.path = "";
                    this.parent = "";
                    this.children = new bbox.ds.Deque();
                    this.debug_report = new api.DebugReport();
                }
                QueryResponseEntry.type = bbox.enc.TypeLibrary.simpleStructure("bbox::debug::api::QueryResponseEntry", QueryResponseEntry)
                    .addMember("path", "std::string")
                    .addMember("parent", "std::string")
                    .addMemberByType("children", bbox.enc.TypeLibrary.stdVectorAsDeque("bbox::debug::api::ChildEntry"))
                    .addMember("debug_report", "bbox::debug::api::DebugReport");
                return QueryResponseEntry;
            }());
            api.QueryResponseEntry = QueryResponseEntry;
            ;
        })(api = debug.api || (debug.api = {}));
    })(debug = bbox.debug || (bbox.debug = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var debug;
    (function (debug) {
        var api;
        (function (api) {
            var QueryResponse = (function () {
                function QueryResponse() {
                    this.results = new bbox.ds.Deque();
                }
                QueryResponse.type = bbox.enc.TypeLibrary.simpleStructure("bbox::debug::api::QueryResponse", QueryResponse)
                    .addMemberByType("results", bbox.enc.TypeLibrary.stdVectorAsDeque("bbox::debug::api::QueryResponseEntry"));
                return QueryResponse;
            }());
            api.QueryResponse = QueryResponse;
            ;
        })(api = debug.api || (debug.api = {}));
    })(debug = bbox.debug || (bbox.debug = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var debug;
    (function (debug) {
        var api;
        (function (api) {
            var GetOutputRequest = (function () {
                function GetOutputRequest() {
                    this.req_id = 0;
                    this.debug_enables = new bbox.ds.Deque();
                }
                GetOutputRequest.type = bbox.enc.TypeLibrary.simpleStructure("bbox::debug::api::GetOutputRequest", GetOutputRequest)
                    .addMember("req_id", "uint64_t")
                    .addMemberByType("debug_enables", bbox.enc.TypeLibrary.stdVectorAsDeque("std::string"));
                return GetOutputRequest;
            }());
            api.GetOutputRequest = GetOutputRequest;
            ;
        })(api = debug.api || (debug.api = {}));
    })(debug = bbox.debug || (bbox.debug = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var debug;
    (function (debug) {
        var api;
        (function (api) {
            var GetOutputResponse = (function () {
                function GetOutputResponse() {
                    this.next_id = 0;
                    this.reports = new bbox.ds.Deque();
                }
                GetOutputResponse.type = bbox.enc.TypeLibrary.simpleStructure("bbox::debug::api::GetOutputResponse", GetOutputResponse)
                    .addMember("next_id", "uint64_t")
                    .addMemberByType("reports", bbox.enc.TypeLibrary.stdVectorAsDeque("bbox::debug::api::DebugReport"));
                return GetOutputResponse;
            }());
            api.GetOutputResponse = GetOutputResponse;
            ;
        })(api = debug.api || (debug.api = {}));
    })(debug = bbox.debug || (bbox.debug = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var debug;
    (function (debug) {
        var AppState = (function () {
            function AppState() {
            }
            AppState.prototype.AppState = function () {
                this.path = "/";
            };
            AppState.type = bbox.enc.TypeLibrary.simpleStructure("bbox::debug::AppState", AppState)
                .addMember("path", "std::string");
            return AppState;
        }());
        debug.AppState = AppState;
    })(debug = bbox.debug || (bbox.debug = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var debug;
    (function (debug) {
        var OutputView = (function (_super) {
            __extends(OutputView, _super);
            function OutputView(app) {
                var div = document.createElement('div');
                _super.call(this, div);
                this.app = app;
                this.div = div;
                this.div.classList.add("output-view");
            }
            OutputView.prototype.scrollToBottom = function () {
                this.div.scrollTop = this.div.scrollHeight;
            };
            OutputView.prototype.clear = function () {
                this.div.innerHTML = "";
            };
            OutputView.prototype.addMessage = function (message, is_error) {
                var div = document.createElement('div');
                div.classList.add('output-view-item');
                var msg_div = document.createElement('div');
                msg_div.innerText = message;
                if (is_error)
                    msg_div.classList.add('output-view-error');
                else
                    msg_div.classList.add('output-view-message');
                div.appendChild(msg_div);
                this.div.appendChild(div);
                this.scrollToBottom();
            };
            OutputView.prototype.addDebugReport = function (report) {
                var div = document.createElement('div');
                div.classList.add('output-view-item');
                div.innerHTML = '<pre>' + report.contents_html + '</pre>';
                this.div.appendChild(div);
                this.scrollToBottom();
            };
            OutputView.prototype.addQueryResponse = function (entry) {
                var div = document.createElement('div');
                div.classList.add('output-view-item');
                {
                    var line = document.createElement("div");
                    line.innerHTML = "Item: ";
                    line.appendChild(this.createPathLink(entry.path, entry.path));
                    div.appendChild(line);
                }
                if (entry.parent != "") {
                    var line = document.createElement("div");
                    line.innerHTML = "Parent: ";
                    line.appendChild(this.createPathLink(entry.parent, entry.parent));
                    div.appendChild(line);
                }
                for (var j = 0; j < entry.children.size(); ++j) {
                    var child = entry.children.at(j);
                    var line = document.createElement("div");
                    line.innerHTML = "Child: ";
                    line.appendChild(this.createPathLink(child.name + " (" + child.short_description + ")", child.path));
                    div.appendChild(line);
                }
                var content_div = document.createElement("div");
                content_div.innerHTML = "<pre>" + entry.debug_report.contents_html + "</pre>";
                div.appendChild(content_div);
                this.div.appendChild(div);
                this.scrollToBottom();
            };
            OutputView.prototype.createPathLink = function (name, path) {
                var _this = this;
                var href = "/debug/state" + path;
                var link = document.createElement("a");
                link.href = href;
                link.innerText = name;
                link.onclick = function (ev) {
                    ev.preventDefault();
                    _this.app.pushState_QueryState(path);
                };
                return link;
            };
            return OutputView;
        }(bbox.ui.Control));
        debug.OutputView = OutputView;
    })(debug = bbox.debug || (bbox.debug = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var debug;
    (function (debug) {
        var GetOutputQuerier = (function () {
            function GetOutputQuerier(api_base, output_view) {
                var _this = this;
                this.running = false;
                this.api_base = api_base;
                this.output_view = output_view;
                this.next_id = 0;
                this.debug_enables = new bbox.ds.Deque();
                this.query_request = null;
                this.timer = new bbox.ui.Timer(function () { _this.handleTimeout(); });
            }
            GetOutputQuerier.prototype.start = function () {
                if (!this.running) {
                    this.running = true;
                    this.next_id = 0;
                    this.query_request = null;
                    this.timer.stop();
                    this.output_view.addMessage("Starting debug output...", false);
                    this.doGetOutputQuery();
                }
            };
            GetOutputQuerier.prototype.stop = function () {
                if (this.running) {
                    this.running = false;
                    this.next_id = 0;
                    this.query_request = null;
                    this.timer.stop();
                    this.output_view.addMessage("Stopping debug output", false);
                }
            };
            GetOutputQuerier.prototype.setDebugEnables = function (debug_enables) {
                this.debug_enables = debug_enables.shallowClone();
            };
            GetOutputQuerier.prototype.handleTimeout = function () {
                if (this.running
                    && (this.query_request === null)) {
                    this.doGetOutputQuery();
                }
            };
            GetOutputQuerier.prototype.doGetOutputQuery = function () {
                var _this = this;
                this.query_request = new bbox.net.BboxRpcRequest(this.api_base, "get_output", debug.api.GetOutputRequest, debug.api.GetOutputResponse, function (req) {
                    if (_this.query_request === req) {
                        _this.query_request = null;
                        _this.handleQueryResponse(req);
                    }
                });
                var request = new debug.api.GetOutputRequest();
                request.req_id = this.next_id;
                request.debug_enables = this.debug_enables.shallowClone();
                this.timer.stop();
                this.query_request.sendAsync(request);
            };
            GetOutputQuerier.prototype.handleQueryResponse = function (req) {
                if (!req.wasSuccessul()) {
                    this.output_view.addMessage("Debug output failed: " + req.getErrorDescription(), true);
                    this.timer.startSingleShot(5 * 1000);
                    // Since it's failed - we need to indicate
                    // that we're starting again
                    this.next_id = 0;
                }
                else {
                    var output_result = req.getResult();
                    this.next_id = output_result.next_id;
                    for (var i = 0; i < output_result.reports.size(); ++i) {
                        this.output_view.addDebugReport(output_result.reports.at(i));
                    }
                    this.timer.startSingleShot(1000);
                }
            };
            return GetOutputQuerier;
        }());
        debug.GetOutputQuerier = GetOutputQuerier;
    })(debug = bbox.debug || (bbox.debug = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var debug;
    (function (debug) {
        var ControlPane = (function (_super) {
            __extends(ControlPane, _super);
            function ControlPane(app, output_view, get_output_querier) {
                var _this = this;
                var div = document.createElement('div');
                _super.call(this, div);
                this.app = app;
                this.output_view = output_view;
                this.get_output_querier = get_output_querier;
                this.div = div;
                this.div.classList.add("control-pane");
                this.add(new bbox.ui.Span("Controls"));
                var home_btn = new bbox.ui.Button("Home");
                home_btn.onClick(function () { _this.onHomeBtnClicked(); });
                this.add(home_btn);
                var clear_btn = new bbox.ui.Button("Clear");
                clear_btn.onClick(function () { _this.onClearBtnClicked(); });
                this.add(clear_btn);
                var enable_div = new bbox.ui.Div();
                enable_div.addClass("radio-group");
                this.disable_btn = new bbox.ui.Button("Disable");
                this.disable_btn.onClick(function () { _this.onDisableBtnClicked(); });
                this.disable_btn.addClass("selected");
                this.enable_btn = new bbox.ui.Button("Enable");
                this.enable_btn.onClick(function () { _this.onEnableBtnClicked(); });
                this.enables_edit_box = new bbox.ui.EditBox("");
                this.enables_edit_box.onChanged(function () { _this.onEnablesEditBoxChanged(); });
                enable_div.add(this.disable_btn);
                enable_div.add(this.enable_btn);
                this.add(enable_div);
                this.add(this.enables_edit_box);
            }
            ControlPane.prototype.onClearBtnClicked = function () {
                this.output_view.clear();
            };
            ControlPane.prototype.onHomeBtnClicked = function () {
                this.app.pushState_QueryState("/");
            };
            ControlPane.prototype.onDisableBtnClicked = function () {
                this.disable_btn.addClass("selected");
                this.enable_btn.removeClass("selected");
                this.get_output_querier.stop();
            };
            ControlPane.prototype.onEnableBtnClicked = function () {
                this.disable_btn.removeClass("selected");
                this.enable_btn.addClass("selected");
                this.get_output_querier.start();
            };
            ControlPane.prototype.onEnablesEditBoxChanged = function () {
                var enables = new bbox.ds.Deque();
                if (this.enables_edit_box.getValue() != "") {
                    enables.push_back(this.enables_edit_box.getValue());
                }
                this.get_output_querier.setDebugEnables(enables);
            };
            return ControlPane;
        }(bbox.ui.Container));
        debug.ControlPane = ControlPane;
    })(debug = bbox.debug || (bbox.debug = {}));
})(bbox || (bbox = {}));
var bbox;
(function (bbox) {
    var debug;
    (function (debug) {
        var Application = (function (_super) {
            __extends(Application, _super);
            function Application(api_base) {
                _super.call(this, debug.AppState);
                this.api_base = api_base;
                this.query_request = null;
                this.output_view = new debug.OutputView(this);
                this.get_output_querier = new debug.GetOutputQuerier(api_base, this.output_view);
                this.control_pane = new debug.ControlPane(this, this.output_view, this.get_output_querier);
                this.body().add(this.control_pane);
                this.body().add(this.output_view);
            }
            Application.prototype.stateToQuery = function (state) {
                var query = "/";
                if (state
                    && state.path) {
                    query = state.path;
                }
                return query;
            };
            Application.prototype.onLoad = function (state) {
                this.queryState(this.stateToQuery(state));
            };
            Application.prototype.onPopState = function (state) {
                this.queryState(this.stateToQuery(state));
            };
            Application.prototype.decodeUrlToState = function (url) {
                var result = new debug.AppState();
                if (url.substr(0, 13) == "/debug/state/")
                    result.path = url.substr(12);
                return result;
            };
            Application.prototype.pushState_QueryState = function (query) {
                var href = "/debug/state" + query;
                var state = new debug.AppState();
                state.path = query;
                this.pushState(href, state);
                return false;
            };
            Application.prototype.queryState = function (query) {
                var _this = this;
                this.query_request = new bbox.net.BboxRpcRequest(this.api_base, "query_state", debug.api.QueryRequest, debug.api.QueryResponse, function (req) {
                    if (_this.query_request === req) {
                        _this.query_request = null;
                        _this.handleQueryResponse(req);
                    }
                });
                var request = new debug.api.QueryRequest();
                request.query = query;
                this.query_request.sendAsync(request);
            };
            Application.prototype.handleQueryResponse = function (req) {
                if (!req.wasSuccessul()) {
                    this.output_view.addMessage("Error: " + req.getErrorDescription(), true);
                }
                else {
                    var response = req.getResult();
                    if (response.results.size() == 0) {
                        this.output_view.addMessage("Not found", true);
                    }
                    else {
                        for (var i = 0; i < response.results.size(); ++i) {
                            this.output_view.addQueryResponse(response.results.at(i));
                        }
                    }
                }
            };
            return Application;
        }(bbox.ui.Application));
        debug.Application = Application;
    })(debug = bbox.debug || (bbox.debug = {}));
})(bbox || (bbox = {}));
/// <reference path="../../TypeScriptClientLib/lib_references.ts" />
/// <reference path="bbox/debug/api/ChildEntry.ts" />
/// <reference path="bbox/debug/api/DebugReport.ts" />
/// <reference path="bbox/debug/api/QueryRequest.ts" />
/// <reference path="bbox/debug/api/QueryResponseEntry.ts" />
/// <reference path="bbox/debug/api/QueryResponse.ts" />
/// <reference path="bbox/debug/api/GetOutputRequest.ts" />
/// <reference path="bbox/debug/api/GetOutputResponse.ts" />
/// <reference path="bbox/debug/AppState.ts" />
/// <reference path="bbox/debug/OutputView.ts" />
/// <reference path="bbox/debug/GetOutputQuerier.ts" />
/// <reference path="bbox/debug/ControlPane.ts" />
/// <reference path="bbox/debug/Application.ts" />
//# sourceMappingURL=app.js.map