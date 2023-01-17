package CBuilder.list;

import CBuilder.Expression;
import CBuilder.Statement;

public class Append implements Statement {

    /**
     * The list object.
     */
    protected Expression object;

    /**
     * The element that should be appended to the list
     */
    protected Expression element;

    /**
     * Create a new reference for a specific element in a list
     *
     * @param object List object
     * @param index Element object
     */
    public Append(Expression object, Expression element) {
        this.object = object;
        this.element = element;
    }

    @Override
    public String buildStatement() {
        return String.format("__mpy_list_add(%s, %s);", element.buildExpression(), object.buildExpression());
    }
}
