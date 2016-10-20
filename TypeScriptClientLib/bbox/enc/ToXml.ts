namespace bbox.enc
{
    export class ToXml
    {
        private result: XMLDocument;
        private cur_element: Element;
        private element_stack: Element[];
        private has_error: boolean;
        private error: string;
        private state: string;

        constructor(root_node_name: string)
        {
            this.result = document.implementation.createDocument(null, root_node_name, null);
            this.cur_element = this.result.documentElement;
            this.element_stack = [];
            this.has_error = false;
            this.error = "";
            this.state = "Constructed";
        }

        getXmlString(): string
        {
            return (new XMLSerializer()).serializeToString(this.result);
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

        setValue(val: any): void
        {
            if (this.has_error)
            {
                // No-op
            }
            else
            {
                if (val === null)
                {
                    this.raiseError("Can't call setValue with null value");
                }
                else if (typeof val != "string")
                {
                    this.raiseError("Can't call setValue wtih value of type " + typeof val);
                }
                else if (this.state == "Constructed")
                {
                    this.cur_element.setAttribute("value", val);
                    this.state = "Completed-Single-Value";
                }
                else if (this.state == "Started-Named-Value")
                {
                    this.cur_element.setAttribute("value", val);
                    this.state = "Completed-Named-Value";
                }
                else
                {
                    this.raiseError("Can't call setValue in state " + this.state);
                }
            }
        }

        encodeTypedValue(val: any, type_name: string): void
        {
            if (this.has_error)
            {
                // No-op
            }
            else
            {
                var type = TypeLibrary.instance().findType(type_name);
                if (!type)
                {
                    this.raiseError("Unknown type " + type_name);
                }
                else
                {
                    type.toXml(val, this);
                }
            }
        }

        startObject(): void
        {
            if (this.has_error)
            {
                // No-op
            }
            else if (this.state == "Constructed")
            {
                this.state = "Started-Single-Object";
            }
            else if (this.state == "Started-Named-Value")
            {
                this.state = "Started-Named-Object";
            }
            else
            {
                this.raiseError("Can't call startObject in state " + this.state);
            }
        }

        endObject(): void
        {
            if (this.has_error)
            {
                // No-op
            }
            else if (this.state == "Started-Single-Object")
            {
                this.state = "Completed-Single-Value";
            }
            else if (this.state == "Started-Named-Object")
            {
                this.state = "Completed-Named-Value";
            }
            else
            {
                this.raiseError("Can't call endObject in state " + this.state);
            }
        }

        startNamedValue(name: string): void
        {
            if (this.has_error)
            {
                // No-op
            }
            else if ((this.state == "Started-Single-Object")
                     || (this.state == "Started-Named-Object"))
            {
                var parent_element = this.cur_element;
                this.cur_element = this.result.createElement(name);

                this.element_stack.push(parent_element);

                parent_element.appendChild(this.cur_element);

                this.state = "Started-Named-Value";
            }
            else
            {
                this.raiseError("Can't call startNamedValue in state " + this.state);
            }
        }

        endNamedValue(): void
        {
            if (this.has_error)
            {
                // No-op
            }
            else if (this.state == "Completed-Named-Value")
            {
                this.cur_element = this.element_stack.pop();

                if (this.element_stack.length == 0)
                {
                    this.state = "Started-Single-Object";
                }
                else
                {
                    this.state = "Started-Named-Object";
                }
            }
            else
            {
                this.raiseError("Can't call endNamedValue in state " + this.state);
            }
        }
    }
}