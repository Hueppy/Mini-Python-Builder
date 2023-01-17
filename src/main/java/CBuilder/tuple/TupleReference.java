package CBuilder.tuple;

import CBuilder.Expression;

/**
 * Representation of a reference to a tuple element.
 */
public class TupleReference implements Expression {

    /**
     * The tuple object.
     */
    protected Expression object;

    /**
     * The element index
     */
    protected int index;

    /**
     * Create a new reference for a specific element in a tuple
     *
     * @param object Tuple object
     * @param index Element index
     */
    public TupleReference(CBuilder.Expression object, int index) {
        this.object = object;
        this.index = index;
    }


    @Override
    public String buildExpression() {
        return String.format("__mpy_tuple_get_at(%s, %d)", object.buildExpression(), index);
    }

    @Override
    public String buildStatement() {
        return buildExpression() + ";\n";
    }
}
