package CBuilder.list;

import CBuilder.Expression;

public class ListReference implements Expression {

    /**
     * The list object.
     */
    protected Expression object;

    /**
     * The element index
     */
    protected Expression index;

    /**
     * Create a new reference for a specific element in a list
     *
     * @param object List object
     * @param index Element index
     */
    public ListReference(CBuilder.Expression object, Expression index) {
        this.object = object;
        this.index = index;
    }


    @Override
    public String buildExpression() {
        return String.format("__mpy_list_get_at(%s, __mpy_int_value(%s))", object.buildExpression(), index.buildExpression());
    }

    @Override
    public String buildStatement() {
        return buildExpression() + ";\n";
    }
}
