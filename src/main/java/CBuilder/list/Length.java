package CBuilder.list;

import CBuilder.Expression;
import CBuilder.Statement;

public class Length implements Expression {

    /**
     * The list object.
     */
    protected Expression object;

    /**
     * Get size of list
     *
     * @param object List object
     */
    public Length(Expression object) {
        this.object = object;
    }

    @Override
    public String buildExpression() {
        return String.format("__mpy_obj_init_int(__mpy_list_size(%s))", object.buildExpression());
    }

    @Override
    public String buildStatement() {
        return buildExpression() + ";\n";
    }
}
