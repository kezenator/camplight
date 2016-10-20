class SimpleType
{
    static type = bbox.enc.TypeLibrary.simpleStructure("SimpleType", SimpleType)
        .addMember("name", "std::string")
        .addMember("id", "uint32_t")
        .addMemberByType("list", bbox.enc.TypeLibrary.stdVectorAsDeque("std::string"));

    constructor()
    {
        this.name = "";
        this.id = 0;
        this.list = new bbox.ds.Deque<string>();
    }

    public name: string;
    public id: number;
    public list: bbox.ds.Deque<string>;
}

class MyApplication extends bbox.ui.Application
{
    private timer: bbox.ui.Timer;
    private tests: bbox.ds.Deque<string>;
    private cur_test_success: boolean;
    private cur_test_pre: bbox.ui.Pre;

    constructor()
    {
        super();

        this.timer = new bbox.ui.Timer(this.testTimeout.bind(this));

        this.tests = new bbox.ds.Deque<string>();
        this.tests.push_back("testtoxml");
        this.tests.push_back("testfromxml");
    }        

    onload(): void
    {
        this.timer.startPeriodic(100, 100);
    }

    private testTimeout(): void
    {
        if (this.tests.size() == 0)
        {
            this.timer.stop();

            var span = new bbox.ui.Span("All done");
            this.body().add(span);
        }
        else
        {
            var name = this.tests.pop_front();

            var div = new bbox.ui.Div();
            var pre = new bbox.ui.Pre("Test " + name + "\n");

            div.add(pre);
            this.body().add(div);

            this.cur_test_success = true;
            this.cur_test_pre = pre;

            // Run the test
            try
            {
                (<any>this)[name](this);
            }
            catch (err)
            {
                this.cur_test_pre.appendText("Exception: " + err.message);
                this.cur_test_success = false;
            }

            if (this.cur_test_success)
                pre.appendText("Success");
            else
                pre.appendText("Failed");

            this.cur_test_pre = null;
        }
    }

    private log(str: string): void
    {
        this.cur_test_pre.appendText(str + "\n");
    }

    private assert(test: boolean, label: string): void
    {
        if (!test)
        {
            this.log("Assertion failed: " + label);
            this.cur_test_success = false;
        }
    }

    testtoxml(): void
    {
        this.log("Running first test...");

        var simple_value = new SimpleType();
        simple_value.name = "simple_name";
        simple_value.id = 1234;
        simple_value.list.push_back("hello");
        simple_value.list.push_back("world");

        var toxml = new bbox.enc.ToXml("root");
        toxml.startObject();
        toxml.startNamedValue("one");
        toxml.setValue("1");
        toxml.endNamedValue();
        toxml.startNamedValue("three");
        toxml.setValue("3");
        toxml.endNamedValue();
        toxml.startNamedValue("two");
        toxml.startObject();
        toxml.startNamedValue("red");
        toxml.setValue("blue");
        toxml.endNamedValue();
        toxml.endObject();
        toxml.endNamedValue();
        toxml.startNamedValue("four");
        toxml.encodeTypedValue(simple_value, "SimpleType");
        toxml.endNamedValue();
        toxml.endObject();

        this.assert(!toxml.hasError(), "no error");

        if (toxml.hasError())
        {
            this.log("Error: " + toxml.getErrorString());
        }
        else
        {
            this.log("xml: " + toxml.getXmlString());
            this.assert(toxml.getXmlString() === '<root><one value="1"/><three value="3"/><two><red value="blue"/></two><four><name value="simple_name"/><id value="1234"/><list><size value="2"/><entry value="hello"/><entry value="world"/></list></four></root>', "expected val");
        }
    }

    testfromxml(): void
    {
        var fromxml: bbox.enc.FromXml;
        var val: any;

        this.log("=== 123 ===");

        fromxml = new bbox.enc.FromXml('<root value="123"/>', "root");
        val = fromxml.getValue();
        fromxml.checkComplete();

        if (fromxml.hasError())
            this.log("Got error: " + fromxml.getErrorString());

        this.assert(!fromxml.hasError(), "expected no error");
        this.assert(val === "123", "expected getValue returned \"123\" but returned " + typeof val + " " + val);

        this.log("=== simple object ===");

        var v1: any;
        var v3: any;
        var v4: any;
        var v2r: any;
        var v4n: any;
        var v4i: any;
        var deq: any;

        fromxml = new bbox.enc.FromXml(
            '<root><one value="1"/><three value="3"/><two><red value="blue"/></two><four><name value="simple_name"/><id value="1234"/><list><size value="2"/><entry value="hello"/><entry value="world"/></list></four></root>',
            'root');

        fromxml.startObject();
        fromxml.startNamedValue("one");
        v1 = fromxml.getValue();
        fromxml.endNamedValue();
        fromxml.startNamedValue("three");
        v3 = fromxml.getValue();
        fromxml.endNamedValue();
        fromxml.startNamedValue("two");
        fromxml.startObject();
        fromxml.startNamedValue("red");
        v2r = fromxml.getValue();
        fromxml.endNamedValue();
        fromxml.endObject();
        fromxml.endNamedValue();
        fromxml.startNamedValue("four");
        v4 = fromxml.decodeTypedValue("SimpleType");
        fromxml.endNamedValue();
        fromxml.endObject();
        fromxml.checkComplete();

        if (!fromxml.hasError())
        {
            v4n = v4.name;
            v4i = v4.id;
            deq = v4.list;
        }

        if (fromxml.hasError())
            this.log("Got error: " + fromxml.getErrorString());

        this.assert(!fromxml.hasError(), "expected no error");
        if (!fromxml.hasError())
        {
            this.assert(v1 === "1", "error with v1");
            this.assert(v3 === "3", "error with v3");
            this.assert(v2r === "blue", "error with v2r");
            this.assert(v4n === "simple_name", "error with v4n");
            this.assert(v4i === 1234, "error with v4i");
            this.assert(deq.size() === 2, "error with deque size");
            this.assert(deq.at(0) === "hello", "error with deque at 0");
            this.assert(deq.at(1) === "world", "error with deque at 1");
        }
    }
}