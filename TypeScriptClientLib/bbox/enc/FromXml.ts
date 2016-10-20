namespace bbox.enc
{
    class FromXml_ObjectProgress
    {
        element: Element;
        at_start: boolean;
        finished: boolean;
        cur_child: Element;

        constructor(element: Element)
        {
            this.element = element;
            this.at_start = true;
            this.finished = false;
            this.cur_child = null;
        }

        nextElement(): Element
        {
            var result: Element;

            if (this.finished)
            {
                result = null;
            }
            else if (this.at_start)
            {
                result = this.element.firstElementChild;
            }
            else
            {
                result = this.cur_child.nextElementSibling;
            }

            if (result === null)
            {
                this.finished = true;
            }
            else
            {
                this.cur_child = result;
                this.at_start = false;
            }

            return result;
        }
    };

    export class FromXml
    {
        private document: Document;
        private root_element: Element;
        private cur_obj_progress: FromXml_ObjectProgress;
        private parent_stack: ds.Deque<FromXml_ObjectProgress>;
        private has_error: boolean;
        private error: string;
        private state: string;

        constructor(json_str: string, root_element_name: string)
        {
            this.document = null;
            this.root_element = null;
            this.cur_obj_progress = null;
            this.parent_stack = new ds.Deque<FromXml_ObjectProgress>();
            this.has_error = false;
            this.error = "";
            this.state = "Constructed";

            try
            {
                var parser = new DOMParser();
                this.document = parser.parseFromString(json_str, "text/xml");
                this.root_element = this.document.documentElement;
            }
            catch (e)
            {
                this.raiseError("Syntax error parsing XML input");
            }

            if (!this.hasError)
            {
                if (this.root_element.nodeName != root_element_name)
                {
                    this.raiseError("Unexpected root element name - got \"" + this.root_element.nodeName + "\", expected \"" + root_element_name + "\"");
                }
            }
        }

        hasError(): boolean
        {
            return this.has_error;
        }

        getErrorString(): string
        {
            return this.error;
        }

        raiseError(err: string)
        {
            if (!this.has_error)
            {
                this.has_error = true;
                this.error = err;
            }
        }

        checkComplete(): void
        {
            if (this.has_error)
            {
                // No-op
            }
            else if (this.state != "Complete")
            {
                this.raiseError("checkComplete called but in state " + this.state);
            }
        }

        decodeTypedValue(type_name: string): void
        {
            if (this.has_error)
            {
                // No-op
                return undefined;
            }
            else
            {
                var type = TypeLibrary.instance().findType(type_name);
                if (!type)
                {
                    this.raiseError("Unknown type " + type_name);
                    return undefined;
                }
                else
                {
                    return type.fromXml(this);
                }
            }
        }

        public getValue(): any
        {
            var result: any;

            if (this.has_error)
            {
                return undefined;
            }
            else if ((this.state == "Constructed")
                     || (this.state == "Started-Named-Value"))
            {
                var element: Element;

                if (this.state == "Constructed")
                    element = this.root_element;
                else
                    element = this.cur_obj_progress.cur_child;

                if (element.childElementCount != 0)
                {
                    this.raiseError("Expected value but found element \"" + element.nodeName
                        + "\" containing " + element.childElementCount + " child elements including \""
                        + element.firstElementChild.nodeName + "\"");
                    return undefined;
                }
                else if (element.textContent != "")
                {
                    this.raiseError("Expected value but found element \"" + element.nodeName
                        + "\" containing unexpected text content");
                    return undefined;
                }
                else if (!element.hasAttribute("value"))
                {
                    this.raiseError("Expected value but found element \"" + element.nodeName
                        + "\" without \"value\" attribute");
                    return undefined;
                }

                result = element.getAttribute("value");

                if (this.state == "Constructed")
                    this.state = "Complete";
                else
                    this.state = "Completed-Named-Value";
            }
            else
            {
                this.raiseError("Can't call getValue in state " + this.state);
                return undefined;
            }

            if (result === null)
            {
                this.raiseError("Can't call getValue for null objects - all values must be strings");
                return undefined;
            }
            else if (typeof result == "number")
            {
                // Javascript automatically converts strings which are valid numbers -
                // just convert back to a string

                return (<number>(result)).toString();
            }
            else if (typeof result != "string")
            {
                this.raiseError("Can't call getValue for objects of type \"" + typeof result + "\" - all values must be strings");
                return undefined;
            }
            else
            {
                return result;
            }
        }

        public startObject(): void
        {
            var cur_progress: FromXml_ObjectProgress;

            if (this.has_error)
            {
                // No-op
                return;
            }
            else if (this.state == "Constructed")
            {
                cur_progress = new FromXml_ObjectProgress(this.root_element);
            }
            else if (this.state == "Started-Named-Value")
            {
                var cur_progress = new FromXml_ObjectProgress(this.cur_obj_progress.cur_child);
                this.parent_stack.push_back(this.cur_obj_progress);
            }
            else
            {
                this.raiseError("Can't call startObject in state " + this.state);
                return;
            }

            this.cur_obj_progress = cur_progress;
            this.state = "In-Object";
        }

        public endObject(): void
        {
            if (this.has_error)
            {
                // No-op
            }
            else if (this.state == "In-Object")
            {
                var obj_element = this.cur_obj_progress.element;
                var remaining_element = this.cur_obj_progress.nextElement();

                if (remaining_element !== null)
                {
                    this.raiseError("endObject called, but element \"" + obj_element.nodeName + "\" contains more elements, including \""
                        + remaining_element.nodeName + "\"");
                    return;
                }

                if (this.parent_stack.empty())
                {
                    // Finished root object
                    this.state = "Complete";
                }
                else
                {
                    // Finished sub-object
                    this.cur_obj_progress = this.parent_stack.pop_back();
                    this.state = "Completed-Named-Value";
                }
            }
            else
            {
                this.raiseError("Can't call endObject in state " + this.state);
                return;
            }
        }

        public startNamedValue(name: string): void
        {
            if (this.has_error)
            {
                // No-op
            }
            else if (this.state == "In-Object")
            {
                var parent = this.cur_obj_progress.element;
                var element = this.cur_obj_progress.nextElement();

                if (element === null)
                {
                    this.raiseError("startNamedValue called in element \"" + parent.nodeName
                        + "\" expecting element \"" + name + "\" but no elements were remaining");
                }
                else if (element.nodeName != name)
                {
                    this.raiseError("startNamedValue called in element \"" + parent.nodeName
                        + "\" expecting element \"" + name + "\" but got element \"" + element.nodeName + "\"");
                }
                else
                {
                    this.state = "Started-Named-Value";
                }
            }
            else
            {
                this.raiseError("Can't call startNamedValue in state " + this.state);
                return;
            }
        }

        public endNamedValue(): void
        {
            if (this.has_error)
            {
                // No-op
            }
            else if (this.state == "Completed-Named-Value")
            {
                this.state = "In-Object";
            }
            else
            {
                this.raiseError("Can't call endNamedValue in state " + this.state);
                return;
            }
        }
    }
}