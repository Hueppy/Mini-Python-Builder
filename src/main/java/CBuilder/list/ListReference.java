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
    protected int index;

    /**
     * Create a new reference for a specific element in a list
     *
     * @param object List object
     * @param index Element index
     */
    public ListReference(CBuilder.Expression object, int index) {
        this.object = object;
        this.index = index;
    }


    @Override
    public String buildExpression() {
        return String.format("__mpy_list_get_at(%s, %d)", object.buildExpression(), index);
    }

    @Override
    public String buildStatement() {
        return buildExpression() + ";\n";
    }
}
